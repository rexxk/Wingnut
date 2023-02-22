#pragma once


#include "Event/EventQueue.h"
#include "Event/EventBroker.h"

#include "LayerStack.h"
#include "Window.h"

#include "ImGui/ImGuiContext.h"

#include "Input/KeyboardInput.h"
#include "Input/MouseInput.h"

#include "Renderer/Renderer.h"


namespace Wingnut
{

	struct ApplicationProperties
	{
		std::string Title = "Wingnut Engine";
	};


	class Application
	{
	public:
		Application(const ApplicationProperties& properties);
		virtual ~Application();

		void Run();

		void AttachLayer(Ref<Layer> layer);
		void DetachLayer(Ref<Layer> layer);

		void AttachOverlay(Ref<Layer> overlay);
		void DetachOverlay(Ref<Layer> overlay);


		void Terminate();

		std::string GetBaseDirectory() const { return m_BaseDirectory; }

		Ref<EventQueue> GetEventQueue() { return m_EventQueue; }
		Ref<EventBroker> GetEventBroker() { return m_EventBroker; }

		Ref<Renderer> GetRenderer() { return m_Renderer; }

		Ref<Window> GetWindow() { return m_MainWindow; }
		Ref<ImGuiContext> GetUIContext() { return m_ImGuiContext; }

		static Application& Get() { return *s_Instance; }


	private:

		bool m_Running = false;
		bool m_ApplicationMinimized = false;

		std::string m_BaseDirectory = "";

		LayerStack m_LayerStack;

		Ref<Window> m_MainWindow = nullptr;

		Ref<EventBroker> m_EventBroker = nullptr;
		Ref<EventQueue> m_EventQueue = nullptr;

		Ref<Renderer> m_Renderer = nullptr;

		Ref<ImGuiContext> m_ImGuiContext = nullptr;

		Ref<KeyboardInput> m_KeyboardInput = nullptr;
		Ref<MouseInput> m_MouseInput = nullptr;

		inline static Application* s_Instance = nullptr;

	};


}
