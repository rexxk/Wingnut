#include "wingnut_pch.h"
#include "Window.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Application.h"


namespace Wingnut
{


	Ref<Window> Window::Create(const WindowProperties& windowProperties)
	{

		return CreateRef<Window>(windowProperties);
	}


	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);



	Window::Window(const WindowProperties& windowProperties)
		: m_Properties(windowProperties)
	{
		InitWindow();
	}

	Window::~Window()
	{

	}


	void Window::InitWindow()
	{
		WNDCLASSEX wcex = { 0 };
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.hInstance = GetModuleHandle(NULL);
		wcex.lpszClassName = L"WingnutEngine";
		wcex.lpfnWndProc = WndProc;
		wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wcex.hbrBackground = (HBRUSH)GetStockObject(COLOR_BACKGROUND);
		wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);

		if (!RegisterClassEx(&wcex))
		{
			LOG_CORE_ERROR("Unable to register window class 'WingnutEngine'");
			return;
		}

		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);

		wchar_t wcharTitle[128];
		std::mbstowcs(wcharTitle, m_Properties.Title.c_str(), 128);

		m_WindowHandle = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"WingnutEngine", wcharTitle, WS_OVERLAPPEDWINDOW,
			(screenWidth - m_Properties.Width) / 2, (screenHeight - m_Properties.Height) / 2, m_Properties.Width, m_Properties.Height,
			0, 0, GetModuleHandle(NULL), 0);

		RECT rect = { 0 };
		GetClientRect((HWND)m_WindowHandle, &rect);

		rect.left = 0;
		rect.top = 0;
		rect.right = m_Properties.Width;
		rect.bottom = m_Properties.Height;

		AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_OVERLAPPEDWINDOW);

		SetWindowPos((HWND)m_WindowHandle, 0, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE);


		ShowWindow((HWND)m_WindowHandle, SW_SHOW);
	}

	void Window::HandleMessages()
	{
		MSG msg;

		while (PeekMessage(&msg, (HWND)m_WindowHandle, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}


	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_SIZE:
		{
			LOG_CORE_TRACE("Window size: {},{}", LOWORD(lParam), HIWORD(lParam));
			break;
		}

		case WM_DESTROY:
		{
			PostQuitMessage(0);
			Application::Get().Terminate();

			return 0;
		}
		}

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

}
