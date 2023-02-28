#include "wingnut_pch.h"
#include "Application.h"

#include "Event/EventUtils.h"
#include "Event/WindowEvents.h"

#include "Timer.h"


namespace Wingnut
{

	Application::Application(const ApplicationProperties& properties)
	{
		if (s_Instance != nullptr)
		{
			LOG_CORE_ERROR("An instance of the application already exists!");
			return;
		}

		s_Instance = this;

		m_EventBroker = CreateRef<EventBroker>();
		m_EventQueue = CreateRef<EventQueue>();

#ifdef _WIN32

		HMODULE module;
		module = GetModuleHandle(NULL);

		wchar_t baseDirectory[MAX_PATH] = {};
		GetModuleFileName(module, baseDirectory, MAX_PATH);

		char baseDirectoryMB[MAX_PATH] = {};
		std::wcstombs(baseDirectoryMB, baseDirectory, MAX_PATH);

		std::string executablePath = baseDirectoryMB;

		m_BaseDirectory = executablePath.substr(executablePath.find_last_of("/\\") + 1);

#endif


		SubscribeToEvent<WindowClosedEvent>([&](WindowClosedEvent& event)
			{
				LOG_CORE_TRACE("Window closed event received in Application");

				m_Running = false;

				return false;
			});

		SubscribeToEvent<WindowResizedEvent>([=](WindowResizedEvent& event)
			{
				LOG_CORE_TRACE("Window resized: {},{}", event.Width(), event.Height());

				if (event.Width() == 0 || event.Height() == 0)
				{
					m_ApplicationMinimized = true;
				}
				else
				{
					m_ApplicationMinimized = false;
				}

				return false;
			});


		WindowProperties windowProps;
		windowProps.Width = 1280;
		windowProps.Height = 720;
		windowProps.Title = properties.Title;

		m_MainWindow = Window::Create(windowProps);

		m_Renderer = Renderer::Create(m_MainWindow->WindowHandle());

		m_ImGuiContext = ImGuiContext::Create();

		m_KeyboardInput = CreateRef<KeyboardInput>();
		m_MouseInput = CreateRef<MouseInput>();

	}

	Application::~Application()
	{
		m_ImGuiContext->Release();
		m_Renderer->Release();
	}

	void Application::Run() 
	{


		m_Running = true;

		Timestep timeStep;

		Timer fpsTimer;
		uint32_t framesPerSecond = 0;

		while (m_Running)
		{
			m_MainWindow->HandleMessages();
			m_EventQueue->Process();

			timeStep.Update();


			if (!m_ApplicationMinimized)
			{

				Renderer::BeginScene();

				for (Ref<Layer> layer : m_LayerStack)
				{

					layer->OnUpdate(timeStep);

				}

				m_ImGuiContext->NewFrame(timeStep);

				for (Ref<Layer> layer : m_LayerStack)
				{
					layer->OnUIRender();
				}

				m_ImGuiContext->Render();

				Renderer::EndScene();

				Renderer::SubmitQueue();

				Renderer::Present();


				framesPerSecond++;

				if (fpsTimer.ElapsedTime() > 1000.0f)
				{
					LOG_CORE_TRACE("FPS: {}", framesPerSecond);

					fpsTimer.Reset();

					framesPerSecond = 0;
				}
			}
		}


		for (auto& layer : m_LayerStack)
		{
			layer->OnDetach();
		}

	}


	void Application::Terminate()
	{
		LOG_CORE_TRACE("Terminating program");
		m_Running = false;
	}

	void Application::AttachLayer(Ref<Layer> layer)
	{
		m_LayerStack.AttachLayer(layer);
	}

	void Application::DetachLayer(Ref<Layer> layer)
	{
		m_LayerStack.DetachLayer(layer);
	}

	void Application::AttachOverlay(Ref<Layer> overlay)
	{
		m_LayerStack.AttachOverlay(overlay);
	}

	void Application::DetachOverlay(Ref<Layer> overlay)
	{
		m_LayerStack.DetachOverlay(overlay);
	}

}
