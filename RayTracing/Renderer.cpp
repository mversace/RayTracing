#include <numeric>
#include <chrono>
#include <thread>

#include "Renderer.h"
#include "World.h"
#include "RayCamera.h"
#include "LambertianMaterial.h"
#include "MetalMaterial.h"
#include "DielectricMaterial.h"


RayMath::Vec3 rayColor(const Ray& r, const World& world, int depth)
{
	HitRec rec;

	if (depth <= 0) {
		return RayMath::Vec3(0, 0, 0);
	}

	if (world.hit(r, 0.001f, 1000.0f, rec)) {
		Ray scattered;
		RayMath::Vec3 attenuation;
		if (rec.matPtr->scatter(r, rec, attenuation, scattered)) {
			return attenuation * rayColor(scattered, world, depth - 1);
		}
		return RayMath::Vec3(0, 0, 0);
	}

	RayMath::Vec3 direction = r.direction();
	direction.normalize();
	float t = 0.5f * (direction.y() + 1.0f);

	return (1.0f - t) * RayMath::Vec3(1.0f, 1.0f, 1.0f) + t * RayMath::Vec3(0.5f, 0.7f, 1.0f);
}

namespace RayRender
{
	int g_width = 0;
	int g_height = 0;
	// 每个像素的光线数量
	int ns = 100;

	std::vector<int> pixels;
	World world;
	Camera camera;

	// 线程数量
	constexpr int threadCount = 12;
	// 是否关闭线程
	bool isShut = false;
	int threadState[threadCount] = { 0 };


	HDC g_tempDC = nullptr;
	HBITMAP g_tempBm = nullptr;
	HBITMAP g_oldBm = nullptr;
	unsigned int* g_frameBuff = nullptr;

	void renderer(int threadIdx);
}

void RayRender::initRenderer(int w, int h, HWND hWnd)
{
	g_width = w;
	g_height = h;

	camera.init(90.0f, float(w) / float(h));

	// 1. 创建一个屏幕缓冲
	// 1.1 创建一个与当前设备兼容的DC
	HDC hDC = GetDC(hWnd);
	g_tempDC = CreateCompatibleDC(hDC);
	ReleaseDC(hWnd, hDC);
	// 1.2 创建该DC的bitmap缓冲  32位色
	BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), w, h, 1, 32, BI_RGB,
		(DWORD)w * h * 4, 0, 0, 0, 0 } };
	g_tempBm = CreateDIBSection(g_tempDC, &bi, DIB_RGB_COLORS, (void**)&g_frameBuff, 0, 0);
	// 1.3 选择该bitmap到dc中
	g_oldBm = (HBITMAP)SelectObject(g_tempDC, g_tempBm);
	
	// 做一下随机
	pixels.resize(g_height * g_width);
	std::iota(pixels.begin(), pixels.end(), 1);
	auto seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::shuffle(pixels.begin(), pixels.end(), std::default_random_engine(static_cast<unsigned int>(seed)));

	// 初始化场景
	auto material_left = std::make_shared<LambertianMaterial>(RayMath::Vec3(0.0f, 0.0f, 1.0f));
	auto material_right = std::make_shared<LambertianMaterial>(RayMath::Vec3(1.0f, 0.0f, 0.0f));
	auto R = cos(3.141592653f / 4);
	world.addObject(Sphere(RayMath::Vec3(-R, 0.0f, 1.0f), R, material_left));
	world.addObject(Sphere(RayMath::Vec3(R, 0.0f, 1.0f), R, material_right));

	// 起个线程开始光线追踪
	for (int i = 0; i < threadCount; ++i) {
		std::thread t(RayRender::renderer, i);
		t.detach();
	}
}

void RayRender::update(HWND hWnd)
{
	// present frameBuffer to screen
	HDC hDC = GetDC(hWnd);
	BitBlt(hDC, 0, 0, g_width, g_height, g_tempDC, 0, 0, SRCCOPY);
	ReleaseDC(hWnd, hDC);
}

void RayRender::renderer(int threadIdx)
{
	// 反射次数
	int maxDepth = 50;

	for (int idx = threadIdx; idx < pixels.size(); idx += threadCount) {
		int curIdx = pixels[idx];
		int i = curIdx / g_width;
		int j = curIdx % g_width;

		RayMath::Vec3 col{ 0.0f, 0.0f, 0.0f };
		for (auto k = 0; k < ns; ++k) {
			auto u = static_cast<float>(j + RayMath::randF()) / static_cast<float>(g_width - 1);
			auto v = static_cast<float>(i + RayMath::randF()) / static_cast<float>(g_height - 1);
			auto r = camera.getRay(u, v);
			col += rayColor(r, world, maxDepth);
		}
		col /= float(ns);
		auto rgbtRed = static_cast<unsigned char>(255.99 * col.x());
		auto rgbtGreen = static_cast<unsigned char>(255.99 * col.y());
		auto rgbtBlue = static_cast<unsigned char>(255.99 * col.z());

		if (isShut) break;

		g_frameBuff[curIdx] = rgbtRed << 16 | rgbtGreen << 8 | rgbtBlue;
	}

	threadState[threadIdx] = 1;
}

void RayRender::shutDown()
{
	isShut = true;
	bool isStop = false;
	while (!isStop) {
		isStop = true;
		for (int i = 0; i < threadCount; ++i) {
			if (threadState[i] == 0) {
				isStop = false;
			}
		}
	}

	if (g_tempDC)
	{
		if (g_oldBm)
		{
			SelectObject(g_tempDC, g_oldBm);
			g_oldBm = nullptr;
		}
		DeleteDC(g_tempDC);
		g_tempDC = nullptr;
	}

	if (g_tempBm)
	{
		DeleteObject(g_tempBm);
		g_tempBm = nullptr;
	}
}