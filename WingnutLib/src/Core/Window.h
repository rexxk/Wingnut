#pragma once




namespace Wingnut
{


	struct WindowProperties
	{
		uint32_t Width;
		uint32_t Height;
		std::string Title;

		bool StartMaximized = false;

	};


	class Window
	{
	public:
		static Ref<Window> Create(const WindowProperties& windowProperties);

		Window(const WindowProperties& windowProperties);
		~Window();

		void* WindowHandle() { return m_WindowHandle; }

		void HandleMessages();

	private:
		void InitWindow();

	private:

		void* m_WindowHandle;

		WindowProperties m_Properties;

	};



}
