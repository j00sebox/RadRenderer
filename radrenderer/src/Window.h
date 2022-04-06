#pragma once

#include <Windows.h>

LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

class Window {
public:
	Window();
	~Window();

	bool process_messages();
	void set_pixel(int x, int y, COLORREF& color);

	static inline Window* get() { return m_instance; }
	inline HWND get_hwnd() const { return m_hwnd; }

private:
	HINSTANCE m_hinst;
	HWND m_hwnd;

	static Window* m_instance;
};