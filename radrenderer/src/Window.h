#pragma once

#include <Windows.h>

LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

class Window {
public:
	Window();
	Window(const Window&) = delete;
	Window& operator =(const Window&) = delete;
	~Window();

	bool process_messages();

private:
	HINSTANCE m_instance;
	HWND m_hwnd;
};