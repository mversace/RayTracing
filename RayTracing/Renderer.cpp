module;

#include <windows.h>
#include <math.h>
#include <vector>
#include <memory>
#include <thread>
#include <algorithm>
#include <random>
#include <chrono>
#include <numeric>

module Renderer;
import World;

#pragma warning(disable:4005)
#pragma warning(disable:5106)

inline float randF() {
	return static_cast<float>(rand()) / RAND_MAX;
}

RayMath::Vec3 randomInUnitSphere() {
	// 为了防止找到的点进入了内部

	RayMath::Vec3 p;
	do {
		p = 2.0f * RayMath::Vec3(randF(), randF(), randF()) - RayMath::Vec3(1.0f, 1.0f, 1.0f);
	} while (p.squaredLength() >= 1.0f);

	return p;
}

RayMath::Vec3 color(const Ray& r, const World& world)
{
	HitRec rec = { 0 };
	if (world.hit(r, 0.01f, 1000.0f, rec)) {
		// 以入射点为中心 获取单位立方体中随机点
		auto target = rec.p + rec.normal + randomInUnitSphere();
		// 随机点减去入射点 获得随机反射出来的射线 继续参与颜色计算
		return 0.5f * color(Ray(rec.p, target - rec.p), world);
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
	int threadCount = 12;
	// 是否关闭线程
	bool isShut = false;


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
	world.addObject({ RayMath::Vec3(0.0f, 0.0f, -1.0f), 0.5f });
	world.addObject({ RayMath::Vec3(0.0f, -100.5f, -1.0f), 100.0f });

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
	for (int idx = threadIdx; idx < pixels.size(); idx += threadCount) {
		int curIdx = pixels[idx];
		int i = curIdx / g_width;
		int j = curIdx % g_width;

		RayMath::Vec3 col{ 0.0f, 0.0f, 0.0f };
		for (auto k = 0; k < ns; ++k) {
			auto u = static_cast<float>(j + randF()) / static_cast<float>(g_width - 1);
			auto v = static_cast<float>(i + randF()) / static_cast<float>(g_height - 1);
			auto r = camera.getRay(u, v);
			col += color(r, world);
		}
		col /= float(ns);
		auto rgbtRed = static_cast<unsigned char>(255.99 * col.x());
		auto rgbtGreen = static_cast<unsigned char>(255.99 * col.y());
		auto rgbtBlue = static_cast<unsigned char>(255.99 * col.z());

		if (isShut) break;

		g_frameBuff[curIdx] = rgbtRed << 16 | rgbtGreen << 8 | rgbtBlue;
	}
}

void RayRender::shutDown()
{
	isShut = true;
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