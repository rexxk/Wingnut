#include "wingnut_pch.h"
#include "Application.h"



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

	}


	void Application::Run() 
	{


		m_Running = true;


		while (m_Running)
		{

			for (Ref<Layer> layer : m_LayerStack)
			{

				layer->OnUpdate();

			}


			m_Running = false;
		}



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
