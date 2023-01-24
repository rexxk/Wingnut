#include "wingnut_pch.h"
#include "ImGuiContext.h"

#include "Assets/ShaderStore.h"

#include "Event/EventUtils.h"
#include "Event/WindowEvents.h"

#include <imgui.h>


namespace Wingnut
{

	ImGuiContext::ImGuiContext()
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();
		VkExtent2D extent = rendererData.Device->GetDeviceProperties().SurfaceCapabilities.currentExtent;

		ShaderStore::LoadShader("ImGui", "assets/shaders/ImGui.shader");

		m_Renderer = CreateRef<ImGuiRenderer>(extent);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();

		io.BackendPlatformName = "Wingnut";
		io.BackendRendererName = "Wingnut-Vulkan";
		io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		io.ImeWindowHandle = Application::Get().GetWindow()->WindowHandle();

		m_Width = (uint32_t)extent.width;
		m_Height = (uint32_t)extent.height;

		ImGui::StyleColorsDark();


		uint8_t* pixels;
		int32_t atlasWidth;
		int32_t atlasHeight;
		int32_t bytesPerPixel;

		io.Fonts->GetTexDataAsRGBA32(&pixels, &atlasWidth, &atlasHeight, &bytesPerPixel);

		m_AtlasTexture = CreateRef<Vulkan::Texture2D>((uint32_t)atlasWidth, (uint32_t)atlasHeight, (uint32_t)bytesPerPixel, pixels);

		// TextureID = DescriptorSet 1 - should be read from the shader really and not hardcoded (texture localization)
		io.Fonts->SetTexID((ImTextureID)1);


		LOG_CORE_TRACE("[ImGui] Context created");

		SubscribeToEvent<WindowResizedEvent>([&](WindowResizedEvent& event)
			{
				m_Width = event.Width();
				m_Height = event.Height();

				return false;
			});

	}

	ImGuiContext::~ImGuiContext()
	{
		Release();

		ImGui::DestroyContext();
	}

	void ImGuiContext::Release()
	{
		if (m_AtlasTexture)
		{
			m_AtlasTexture->Release();
		}

		if (m_Renderer)
		{
			m_Renderer->Release();
		}
	}

	void ImGuiContext::NewFrame()
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ::ImVec2((float)m_Width, (float)m_Height);

		ImGui::NewFrame();
	}

	void ImGuiContext::Render()
	{
		uint32_t currentFrame = Renderer::GetContext()->GetCurrentFrame();

		ImGui::EndFrame();

		ImGui::Render();

		m_Renderer->BeginScene(currentFrame);



		m_Renderer->EndScene();

		m_Renderer->SubmitQueue();
	}

}
