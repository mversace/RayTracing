module;

#include <windows.h>

export module Renderer;

export namespace SoftRender
{
	// ��ʼ����Ⱦ�� ��Ļ���� ��Ļ����
	void initRenderer(int w, int h, HWND hWnd);
	// ÿ֡����
	void update(HWND hWnd);
	void shutDown();
}