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

		m_Renderer = CreateRef<Renderer>(m_MainWindow->WindowHandle());

	}

	Application::~Application()
	{
		m_Renderer->Release();
	}

	void Application::Run() 
	{


		m_Running = true;


		Timer fpsTimer;
		uint32_t framesPerSecond = 0;

		while (m_Running)
		{
			m_MainWindow->HandleMessages();
			m_EventQueue->Process();


			if (!m_ApplicationMinimized)
			{

				for (Ref<Layer> layer : m_LayerStack)
				{

					layer->OnUpdate();

				}

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
