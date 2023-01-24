#include "wingnut_pch.h"
#include "ImGuiContext.h"

#include "Assets/ShaderStore.h"



namespace Wingnut
{

	ImGuiContext::ImGuiContext()
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();

		ShaderStore::LoadShader("ImGui", "assets/shaders/ImGui.shader");

		m_Renderer = CreateRef<ImGuiRenderer>(rendererData.Device->GetDeviceProperties().SurfaceCapabilities.currentExtent);

	}

	ImGuiContext::~ImGuiContext()
	{
		Release();
	}

	void ImGuiContext::Release()
	{
		if (m_Renderer)
		{
			m_Renderer->Release();
		}
	}

	void ImGuiContext::NewFrame()
	{

	}

	void ImGuiContext::Render()
	{

	}

}
