#include "wingnut_pch.h"
#include "Application.h"

#include "Event/EventUtils.h"
#include "Event/WindowEvents.h"


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

				return false;
			});


		WindowProperties windowProps;
		windowProps.Width = 1280;
		windowProps.Height = 720;
		windowProps.Title = properties.Title;

		m_MainWindow = Window::Create(windowProps);

		m_Renderer = CreateRef<Renderer>(m_MainWindow->WindowHandle());

	}


	void Application::Run() 
	{


		m_Running = true;


		while (m_Running)
		{
			m_MainWindow->HandleMessages();
			m_EventQueue->Process();

			for (Ref<Layer> layer : m_LayerStack)
			{

				layer->OnUpdate();

			}


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
