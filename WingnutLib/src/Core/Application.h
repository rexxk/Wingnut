#pragma once


#include "Event/EventQueue.h"
#include "Event/EventBroker.h"

#include "LayerStack.h"
#include "Window.h"

#include "File/VirtualFileSystem.h"

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


	struct ApplicationMetrics
	{
		uint32_t FPS;

		float TotalFrameTime = 0.0f;
		float MessageHandlingTime = 0.0f;
		float LayerUpdateTime = 0.0f;
		float SceneDrawTime = 0.0f;
		float RenderingTime = 0.0f;
		float UIRenderingTime = 0.0f;
		float GPUTime = 0.0f;

		uint32_t PolygonCount = 0;
	};


	class Application
	{
	public:
		Application(const ApplicationProperties& properties);
		virtual ~Application();

		void Exit();

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

		Ref<VirtualFileSystem> GetVirtualFileSystem() { return m_VirtualFileSystem; }

		ApplicationMetrics& GetMetrics();

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

		Ref<VirtualFileSystem> m_VirtualFileSystem = nullptr;

		inline static Application* s_Instance = nullptr;

	};


}
