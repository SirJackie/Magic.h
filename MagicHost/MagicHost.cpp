﻿#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <time.h>
#include "Main.h"
#include "FPSCalculator.h"

// 定义窗口大小
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// 定义图像大小
const int IMAGE_WIDTH = 800;
const int IMAGE_HEIGHT = 600;

// 定义图像数据
const int IMAGE_SIZE = IMAGE_WIDTH * IMAGE_HEIGHT * 4; // RGBA 格式
BYTE image_Store[IMAGE_SIZE] = { 0 }; // 初始化为全黑
BYTE *image = &image_Store[0];

// ----------

bool firstFrame = true;
bool wannaUpdate = false;
bool wannaExit = false;
FPSCalculator fpsCalculator;

#define NRRW_BUFFER_LEN 1000
#define WIDE_BUFFER_LEN 1000 * sizeof(wchar_t)  // On Windows: 2000

char windowTitleBuffer[NRRW_BUFFER_LEN];
wchar_t wideBuffer[WIDE_BUFFER_LEN];

char keyboardBuffer[256];


// 渲染函数
void Render(HWND hwnd) {
	// 获取窗口 DC
	HDC hdc = GetDC(hwnd);

	// 创建一个内存 DC
	HDC memDC = CreateCompatibleDC(hdc);

	// 创建一个位图
	BITMAPINFO bmi;
	memset(&bmi, 0, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = IMAGE_WIDTH;
	bmi.bmiHeader.biHeight = -IMAGE_HEIGHT;  // 负值表示顶部原点
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	// 创建位图，并选入内存DC
	HBITMAP hBitmap = CreateDIBSection(memDC, &bmi, DIB_RGB_COLORS, NULL, NULL, 0);
	SelectObject(memDC, hBitmap);

	// 将图像数据写入内存DC
	SetDIBits(memDC, hBitmap, 0, IMAGE_HEIGHT, image, &bmi, DIB_RGB_COLORS);

	// 将内存DC的内容绘制到窗口DC上
	BitBlt(hdc, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, memDC, 0, 0, SRCCOPY);

	// 释放资源
	DeleteObject(hBitmap);
	DeleteDC(memDC);

	// 释放窗口 DC
	ReleaseDC(hwnd, hdc);
}

// 主消息处理函数
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_PAINT:
		Render(hwnd);
		ValidateRect(hwnd, NULL);
		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		keyboardBuffer[wParam] = 1;
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	case WM_KEYUP:
		keyboardBuffer[wParam] = 0;
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	
	// 清空键盘Buffer
	memset(keyboardBuffer, 0, 256);

	// 注册窗口类
	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"Win32Demo";
	RegisterClass(&wc);

	// 创建窗口

	// 设置客户区大小为800x600
	RECT rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	int winXPos = 50, winYPos = 50;

	// 根据调整后的窗口大小创建窗口
	HWND hwnd = CreateWindowEx(0, L"Win32Demo", L"Magic.h Graphical Interface | Made with LOVE by @SirJackie (GitHub)",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, winXPos, winYPos,
		rect.right - rect.left, rect.bottom - rect.top,
		NULL, NULL, hInstance, NULL);

	if (hwnd == NULL) {
		return 0;
	}

	// 游戏循环
	MSG msg;
	while (true) {
		// 处理消息
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				return 0;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (firstFrame) {
			Setup(hwnd, &wannaUpdate);
			if (wannaUpdate == true) {
				firstFrame = false;
				// Only Terminate First Frame when it "wannaUpdate".
			}
		}
		else {
			Update(hwnd, &wannaExit);
		}

		// 渲染画面
		Render(hwnd);

		// Show FPS as Window-Title-String
		sprintf_s(windowTitleBuffer, 1000, "Magic.h Graphical Interface | Made with LOVE by @SirJackie (GitHub) | FPS: %f", fpsCalculator.GetCurrentFPS());
		MultiByteToWideChar(CP_ACP, 0, windowTitleBuffer, -1, wideBuffer, WIDE_BUFFER_LEN);
		SetWindowText(hwnd, wideBuffer);

		if (wannaExit == true) {
			Exit();
			break;
		}
	}

	return 0;
}
