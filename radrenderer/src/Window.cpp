#include "Window.h"

#include <stdio.h>

Window* Window::m_instance = nullptr;

// REMOVE LATER
static COLORREF redColor = RGB(255, 0, 0);
static COLORREF blueColor = RGB(0, 0, 255);
static COLORREF greenColor = RGB(0, 255, 0);
void drawLine()
{
	for (int i = 0; i < 300; i++)
		Window::get()->set_pixel(10 + i, 100, blueColor);
}

LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	HBRUSH hbrWhite, hbrGray;

	switch (msg)
	{
		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_PAINT:
		{

			if (Window::get()->get_hwnd())
			{
				drawLine();
			}
	
			return 0;
		}
		
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

Window::Window()
	: m_hinst(GetModuleHandle(nullptr))
{
	if (m_instance)
		__debugbreak();

	m_instance = this;

	const wchar_t* class_name = L"Window Class";

	WNDCLASS wnd_class = {};
	wnd_class.lpszClassName = class_name;
	wnd_class.hInstance = m_hinst;
	wnd_class.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wnd_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	wnd_class.lpfnWndProc = window_proc;
	wnd_class.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));

	if (!RegisterClass(&wnd_class))
	{
		int nResult = GetLastError();
		MessageBox(NULL,
			TEXT("Window class creation failed"),
			TEXT("Window Class Failed"),
			MB_ICONERROR);
	}

	DWORD style = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

	int width = 640;
	int height = 480;

	RECT rect;
	rect.left = 250;
	rect.top = 250;
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;

	AdjustWindowRect(&rect, style, false);

	m_hwnd = CreateWindowEx(
		0,
		class_name,
		L"Rad Renderer",
		style,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top,
		NULL,
		NULL,
		m_hinst,
		NULL
	);

	if (!m_hwnd)
	{
		int nResult = GetLastError();
		MessageBox(NULL,
			TEXT("Window creation failed"),
			TEXT("Window Failed"),
			MB_ICONERROR);
	} 
	else
	{
		ShowWindow(m_hwnd, SW_SHOW);
	}
}

Window::~Window()
{
	const wchar_t* class_name = L"Window Class";

	UnregisterClass(class_name, m_hinst);
}

bool Window::process_messages()
{
	MSG msg = {};

	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return false;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}

void Window::set_pixel(int x, int y, COLORREF& color)
{
	if (m_hwnd == NULL)
	{
		exit(0);
	}
	HDC hdc = GetDC(m_hwnd);
	SetPixel(hdc, x, y, color);
	ReleaseDC(m_hwnd, hdc);
	return;
}

