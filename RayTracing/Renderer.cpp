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
	// Ϊ�˷�ֹ�ҵ��ĵ�������ڲ�

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
		// �������Ϊ���� ��ȡ��λ�������������
		auto target = rec.p + rec.normal + randomInUnitSphere();
		// ������ȥ����� ������������������� ����������ɫ����
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
	// ÿ�����صĹ�������
	int ns = 100;

	std::vector<int> pixels;
	World world;
	Camera camera;

	// �߳�����
	int threadCount = 12;
	// �Ƿ�ر��߳�
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

	// 1. ����һ����Ļ����
	// 1.1 ����һ���뵱ǰ�豸���ݵ�DC
	HDC hDC = GetDC(hWnd);
	g_tempDC = CreateCompatibleDC(hDC);
	ReleaseDC(hWnd, hDC);
	// 1.2 ������DC��bitmap����  32λɫ
	BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), w, h, 1, 32, BI_RGB,
		(DWORD)w * h * 4, 0, 0, 0, 0 } };
	g_tempBm = CreateDIBSection(g_tempDC, &bi, DIB_RGB_COLORS, (void**)&g_frameBuff, 0, 0);
	// 1.3 ѡ���bitmap��dc��
	g_oldBm = (HBITMAP)SelectObject(g_tempDC, g_tempBm);
	
	// ��һ�����
	pixels.resize(g_height * g_width);
	std::iota(pixels.begin(), pixels.end(), 1);
	auto seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::shuffle(pixels.begin(), pixels.end(), std::default_random_engine(static_cast<unsigned int>(seed)));

	// ��ʼ������
	world.addObject({ RayMath::Vec3(0.0f, 0.0f, -1.0f), 0.5f });
	world.addObject({ RayMath::Vec3(0.0f, -100.5f, -1.0f), 100.0f });

	// ����߳̿�ʼ����׷��
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