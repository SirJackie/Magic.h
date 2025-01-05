#include <windows.h>
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

RECT thisTextPos;
UINT thisTextLayout;

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

	// 将图像数据写入内存DC (image参数，就是在Setup和Update里修改的帧缓冲)
	SetDIBits(memDC, hBitmap, 0, IMAGE_HEIGHT, image, &bmi, DIB_RGB_COLORS);

	// -----

	// 处理新增字体创建
	if (invokeInternalFontCreator == 1) {

		// 如果之前，这个字体指针已经创建过字体
		if (fontEnabled[invokeFontChannel] == true) {
			SelectObject(memDC, defaultFontPtr);        // 恢复默认字体
			DeleteObject(fontPtrs[invokeFontChannel]);  // 删除字体对象
			fontEnabled[invokeFontChannel] = false;     // 指针标识恢复空状态
		}

		// 创建新字体
		fontPtrs[invokeFontChannel] = CreateFont(
			-invokeFontSize,                       // 字体高度（负值表示以像素为单位）
			0,                                     // 字体宽度（0 表示自动计算）
			invokeFontItalic ? 12 : 0,             // 字体的倾斜角度
			0,                                     // 字体的基线角度
			invokeFontBold ? FW_BOLD : FW_NORMAL,  // 字体粗细
			invokeFontItalic ? TRUE : FALSE,       // 是否斜体
			invokeFontUnderline ? TRUE : FALSE,    // 是否有下划线
			FALSE,                                 // 是否有删除线
			DEFAULT_CHARSET,                       // 字符集
			OUT_DEFAULT_PRECIS,                    // 输出精度
			CLIP_DEFAULT_PRECIS,                   // 裁剪精度
			DEFAULT_QUALITY,                       // 输出质量
			DEFAULT_PITCH | FF_SWISS,              // 字体间距和族类
			invokeFontFamily                       // 字体名称
		);
		fontEnabled[invokeFontChannel] = true;     // 指针标识设置满状态

		// 启用字体，为了得到并保存默认字体
		defaultFontPtr = (HFONT)SelectObject(memDC, fontPtrs[invokeFontChannel]);

		// 处理完毕，禁用invoke信号
		invokeInternalFontCreator = 0;
	}

	// 处理原有字体删除
	if (invokeInternalFontDeletor == 1) {

		// 如果之前，这个字体指针已经创建过字体
		if (fontEnabled[invokeFontChannel] == true) {
			SelectObject(memDC, defaultFontPtr);        // 恢复默认字体
			DeleteObject(fontPtrs[invokeFontChannel]);  // 删除字体对象
			fontEnabled[invokeFontChannel] = false;     // 指针标识恢复空状态
		}

		// 处理完毕，禁用invoke信号
		invokeInternalFontDeletor = 0;
	}

	// 依次绘制Magic文字接口传输过来的文字

	for (int i = 0; i < MAX_TEXT_CHANNELS; i++) {
		if (textEnabled[i]) {

			// 决定文本字体
			if (fontEnabled[i] == true) {
				SelectObject(memDC, fontPtrs[i]);
			}
			else {
				SelectObject(memDC, defaultFontPtr);
			}

			// 决定文本颜色
			SetTextColor(
				memDC, RGB(
					textColorR[i],
					textColorG[i],
					textColorB[i]
				)
			);

			// 决定背景颜色
			SetBkColor(
				memDC, RGB(
					bgColorR[i],
					bgColorG[i],
					bgColorB[i]
				)
			);

			// 决定背景透明
			if (bgTransparent[i]) {
				SetBkMode(memDC, TRANSPARENT);
			}
			else {
				SetBkMode(memDC, OPAQUE);
			}

			// 决定文本位置
			thisTextPos.left = textX[i];
			thisTextPos.top = textY[i];
			thisTextPos.right = textX[i] + textWidth[i];
			thisTextPos.bottom = textY[i] + textHeight[i];
			
			// 决定文字格式
			thisTextLayout = 0;  // Important!
			switch (textLayout[i]) {
			case CLIP:
				thisTextLayout = NULL;
				break;
			case WORDBREAK:
				thisTextLayout = DT_WORDBREAK;
				break;
			case ELLIPSIS:
				thisTextLayout = DT_END_ELLIPSIS;
				break;
			case CENTER:
				thisTextLayout = DT_CENTER;
				break;
			case SINGLE_LINE:
				thisTextLayout = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
				break;
			}

			// 绘制文本
			DrawTextW(memDC, textPointers[i], -1, &thisTextPos, thisTextLayout);
		}
	}

	// -----

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
