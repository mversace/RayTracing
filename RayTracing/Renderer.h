#pragma once

#include <windows.h>

namespace RayRender
{
	// ��ʼ����Ⱦ�� ��Ļ���� ��Ļ����
	void initRenderer(int w, int h, HWND hWnd);
	// ÿ֡����
	void update(HWND hWnd);
	void shutDown();
}