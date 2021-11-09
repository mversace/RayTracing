#pragma once

#include <windows.h>

namespace RayRender
{
	// 初始化渲染器 屏幕长宽 屏幕缓冲
	void initRenderer(int w, int h, HWND hWnd);
	// 每帧绘制
	void update(HWND hWnd);
	void shutDown();
}