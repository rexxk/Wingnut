#pragma once


#include "LayerStack.h"
#include "Window.h"


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

		void Run();

		void AttachLayer(Ref<Layer> layer);
		void DetachLayer(Ref<Layer> layer);

		void AttachOverlay(Ref<Layer> overlay);
		void DetachOverlay(Ref<Layer> overlay);


		void Terminate();


		static Application& Get() { return *s_Instance; }


	private:

		bool m_Running = false;

		LayerStack m_LayerStack;

		Ref<Window> m_MainWindow = nullptr;


		inline static Application* s_Instance = nullptr;

	};


}
