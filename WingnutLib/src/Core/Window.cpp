#include "wingnut_pch.h"
#include "Window.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Application.h"

#include "Event/EventUtils.h"
#include "Event/KeyboardEvents.h"
#include "Event/MouseEvents.h"
#include "Event/WindowEvents.h"

#include "Input/KeyDefs.h"


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
			Ref<WindowResizedEvent> event = CreateRef<WindowResizedEvent>((uint32_t)LOWORD(lParam), (uint32_t)HIWORD(lParam));
//			AddEventToQueue(event);
			ExecuteEvent(event);

			break;
		}

		case WM_DESTROY:
		{
			PostQuitMessage(0);
//			Application::Get().Terminate();

			Ref<WindowClosedEvent> event = CreateRef<WindowClosedEvent>();
			AddEventToQueue(event);

			return 0;
		}

		case WM_LBUTTONDOWN:
		{
			Ref<MouseButtonPressedEvent> event = CreateRef<MouseButtonPressedEvent>(MouseButton::Left, (uint32_t)LOWORD(lParam), (uint32_t)HIWORD(lParam));
			AddEventToQueue(event);

			break;
		}

		case WM_RBUTTONDOWN:
		{
			Ref<MouseButtonPressedEvent> event = CreateRef<MouseButtonPressedEvent>(MouseButton::Right, (uint32_t)LOWORD(lParam), (uint32_t)HIWORD(lParam));
			AddEventToQueue(event);

			break;
		}

		case WM_MBUTTONDOWN:
		{
			Ref<MouseButtonPressedEvent> event = CreateRef<MouseButtonPressedEvent>(MouseButton::Middle, (uint32_t)LOWORD(lParam), (uint32_t)HIWORD(lParam));
			AddEventToQueue(event);

			break;
		}

		case WM_LBUTTONUP:
		{
			Ref<MouseButtonReleasedEvent> event = CreateRef<MouseButtonReleasedEvent>(MouseButton::Left, (uint32_t)LOWORD(lParam), (uint32_t)HIWORD(lParam));
			AddEventToQueue(event);

			break;
		}

		case WM_RBUTTONUP:
		{
			Ref<MouseButtonReleasedEvent> event = CreateRef<MouseButtonReleasedEvent>(MouseButton::Right, (uint32_t)LOWORD(lParam), (uint32_t)HIWORD(lParam));
			AddEventToQueue(event);

			break;
		}

		case WM_MBUTTONUP:
		{
			Ref<MouseButtonReleasedEvent> event = CreateRef<MouseButtonReleasedEvent>(MouseButton::Middle, (uint32_t)LOWORD(lParam), (uint32_t)HIWORD(lParam));
			AddEventToQueue(event);

			break;
		}

		case WM_MOUSEMOVE:
		{
			Ref<MouseMovedEvent> event = CreateRef<MouseMovedEvent>((uint32_t)LOWORD(lParam), (uint32_t)HIWORD(lParam));
			AddEventToQueue(event);

			break;
		}

		case WM_MOUSEWHEEL:
		{
			Ref<MouseWheelEvent> event = CreateRef<MouseWheelEvent>((int32_t)GET_WHEEL_DELTA_WPARAM(wParam) / 120, (uint32_t)LOWORD(lParam), (uint32_t)HIWORD(lParam));
			AddEventToQueue(event);

			break;
		}

		case WM_KEYDOWN:
		{
			Ref<KeyPressedEvent> event = CreateRef<KeyPressedEvent>((uint32_t)wParam, (uint32_t)LOWORD(lParam));
			AddEventToQueue(event);

			break;
		}

		case WM_SYSKEYDOWN:
		{
			Ref<KeyPressedEvent> event = CreateRef<KeyPressedEvent>((uint32_t)wParam, (uint32_t)LOWORD(lParam));
			AddEventToQueue(event);

			break;
		}

		case WM_KEYUP:
		{
			Ref<KeyReleasedEvent> event = CreateRef<KeyReleasedEvent>((uint32_t)wParam);
			AddEventToQueue(event);

			break;
		}

		case WM_SYSKEYUP:
		{
			Ref<KeyReleasedEvent> event = CreateRef<KeyReleasedEvent>((uint32_t)wParam);
			AddEventToQueue(event);

			break;
		}

		case WM_CHAR:
		{
			Ref<KeyTypedEvent> event = CreateRef<KeyTypedEvent>((uint32_t)wParam);
			AddEventToQueue(event);

			break;
		}

		case WM_SYSCHAR:
		{
			Ref<KeyTypedEvent> event = CreateRef<KeyTypedEvent>((uint32_t)wParam);
			AddEventToQueue(event);

			break;
		}

		}

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

}
