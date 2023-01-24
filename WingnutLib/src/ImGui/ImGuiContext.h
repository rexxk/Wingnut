#pragma once

#include "Renderer/ImGuiRenderer.h"


namespace Wingnut
{

	class ImGuiContext
	{
	public:
		ImGuiContext();
		~ImGuiContext();

		void Release();

		void NewFrame();
		void Render();


	private:
		Ref<ImGuiRenderer> m_Renderer = nullptr;

	};


}
