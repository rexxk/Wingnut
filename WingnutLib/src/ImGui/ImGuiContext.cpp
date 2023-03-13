#include "wingnut_pch.h"
#include "ImGuiContext.h"

#include "Assets/ResourceManager.h"

#include "Event/EventUtils.h"
#include "Event/KeyboardEvents.h"
#include "Event/MouseEvents.h"
#include "Event/WindowEvents.h"

#include "Scene/Components.h"

#include <imgui.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



namespace Wingnut
{

	struct ImGuiScaleTranslate
	{
		glm::vec2 Scale;
		glm::vec2 Translate;
	};



	static ImGuiScaleTranslate s_ScaleTranslateDescriptor;



	uint32_t MouseButtonToImGuiMouseButton(MouseButton button)
	{
		switch (button)
		{
			case MouseButton::Left: return ImGuiMouseButton_Left;
			case MouseButton::Right: return ImGuiMouseButton_Right;
			case MouseButton::Middle: return ImGuiMouseButton_Middle;
		}

		return ImGuiMouseButton_Left;
	}


	Ref<ImGuiContext> ImGuiContext::Create()
	{
		return CreateRef<ImGuiContext>();
	}


	ImGuiContext::ImGuiContext()
	{
		if (s_Instance != nullptr)
		{
			return;
		}

		s_Instance = this;

		auto& rendererData = Renderer::GetContext()->GetRendererData();
		VkExtent2D extent = rendererData.Device->GetDeviceProperties().SurfaceCapabilities.currentExtent;

//		ShaderStore::LoadShader(ShaderType::ImGui, "assets/shaders/ImGui.shader");
		m_Renderer = ImGuiRenderer::Create(extent);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();

		io.IniFilename = "assets/ui.ini";

		io.BackendPlatformName = "Wingnut";
		io.BackendRendererName = "Wingnut-Vulkan";
		io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
//		io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		io.ImeWindowHandle = Application::Get().GetWindow()->WindowHandle();

		m_Width = (uint32_t)extent.width;
		m_Height = (uint32_t)extent.height;

		ImGui::StyleColorsDark();

		io.DisplaySize = ImVec2((float)m_Width, (float)m_Height);
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

		io.KeyMap[ImGuiKey_Backspace] = WK_BACKSPACE;
		io.KeyMap[ImGuiKey_Space] = WK_SPACE;
		io.KeyMap[ImGuiKey_LeftArrow] = WK_LEFT;
		io.KeyMap[ImGuiKey_UpArrow] = WK_UP;
		io.KeyMap[ImGuiKey_RightArrow] = WK_RIGHT;
		io.KeyMap[ImGuiKey_DownArrow] = WK_DOWN;
		io.KeyMap[ImGuiKey_Enter] = WK_ENTER;

		uint8_t* pixels;
		int32_t atlasWidth;
		int32_t atlasHeight;
		int32_t bytesPerPixel;

		io.Fonts->AddFontFromFileTTF("assets/fonts/Carlito-Regular.ttf", 16);
//		io.Fonts->AddFontFromFileTTF("assets/fonts/Carlito-Italic.ttf", 16);
//		io.Fonts->AddFontFromFileTTF("assets/fonts/Carlito-Bold.ttf", 16);

		io.Fonts->GetTexDataAsRGBA32(&pixels, &atlasWidth, &atlasHeight, &bytesPerPixel);

		m_AtlasTexture = Vulkan::Texture2D::Create((uint32_t)atlasWidth, (uint32_t)atlasHeight, (uint32_t)bytesPerPixel, pixels, Vulkan::TextureFormat::R8G8B8A8_Normalized);
		m_AtlasDescriptor = Vulkan::Descriptor::Create(rendererData.Device, ResourceManager::GetShader(ShaderType::ImGui), ImGuiTextureDescriptor);
		m_AtlasDescriptor->SetImageBinding(0, m_AtlasTexture, ResourceManager::GetSampler(SamplerType::Default));
		m_AtlasDescriptor->UpdateBindings();

		io.Fonts->SetTexID((ImTextureID)m_AtlasDescriptor->GetDescriptor());

		m_EntityRegistry = CreateRef<ECS::Registry>();
		m_ImGuiEntity = ECS::EntitySystem::Create(m_EntityRegistry);
		ECS::EntitySystem::AddComponent<TagComponent>(m_ImGuiEntity, "ImGui");

		m_CameraBuffer = Vulkan::UniformBuffer::Create(rendererData.Device, sizeof(ImGuiScaleTranslate));
		m_CameraDescriptor = Vulkan::Descriptor::Create(rendererData.Device, ResourceManager::GetShader(ShaderType::ImGui), ImGuiCameraDescriptor);
		m_CameraDescriptor->SetBufferBinding(0, m_CameraBuffer);
		m_CameraDescriptor->UpdateBindings();

		LOG_CORE_TRACE("[ImGui] Context created");

		SubscribeToEvent<WindowResizedEvent>([&](WindowResizedEvent& event)
			{
				m_Width = event.Width();
				m_Height = event.Height();

				return false;
			});

		SubscribeToEvent<MouseButtonPressedEvent>([&](MouseButtonPressedEvent& event)
			{
				ImGuiIO& io = ImGui::GetIO();
				io.MouseDown[MouseButtonToImGuiMouseButton(event.GetMouseButton())] = true;

				return false;
			});

		SubscribeToEvent<MouseButtonReleasedEvent>([&](MouseButtonReleasedEvent& event)
			{
				ImGuiIO& io = ImGui::GetIO();
				io.MouseDown[MouseButtonToImGuiMouseButton(event.GetMouseButton())] = false;

				return false;
			});

		SubscribeToEvent<MouseMovedEvent>([&](MouseMovedEvent& event)
			{
				ImGuiIO& io = ImGui::GetIO();
				io.MousePos = ImVec2((float)event.PositionX(), (float)event.PositionY());

				return false;
			});

		SubscribeToEvent<MouseWheelEvent>([&](MouseWheelEvent& event)
			{
				ImGuiIO& io = ImGui::GetIO();
				io.MouseWheel += event.Delta();

				return false;
			});

		SubscribeToEvent<KeyPressedEvent>([&](KeyPressedEvent& event)
			{
				ImGuiIO& io = ImGui::GetIO();
				io.KeysDown[event.Key()] = true;

//				LOG_CORE_WARN("KeyDown: {}", event.Key());

				return false;
			});

		SubscribeToEvent<KeyReleasedEvent>([&](KeyReleasedEvent& event)
			{
				ImGuiIO& io = ImGui::GetIO();
				io.KeysDown[event.Key()] = false;

//				LOG_CORE_WARN("KeyUp: {}", event.Key());

				return false;
			});

		SubscribeToEvent<KeyTypedEvent>([&](KeyTypedEvent& event)
			{
				ImGuiIO& io = ImGui::GetIO();
				io.AddInputCharacter(event.Key());

//				LOG_CORE_WARN("KeyTyped: {}", event.Key());

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
		if (m_CameraDescriptor)
		{
			m_CameraDescriptor->Release();
		}

		if (m_AtlasTexture)
		{
			m_AtlasTexture->Release();
		}

		if (m_CameraBuffer)
		{
			m_CameraBuffer->Release();
		}

		if (m_Renderer)
		{
			m_Renderer->Release();
		}
	}

	void ImGuiContext::NewFrame(Timestep ts)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ::ImVec2((float)m_Width, (float)m_Height);
		io.DeltaTime = ts;

		ImGui::NewFrame();
	}

	void ImGuiContext::Render()
	{
		uint32_t currentFrame = Renderer::GetContext()->GetCurrentFrame();

		ImGui::EndFrame();

		ImGui::Render();

		m_Renderer->BeginScene(currentFrame);

		ImDrawData* drawData = ImGui::GetDrawData();

		float left = drawData->DisplayPos.x;
		float right = drawData->DisplayPos.x + drawData->DisplaySize.x;
		float top = drawData->DisplayPos.y;
		float bottom = drawData->DisplayPos.y + drawData->DisplaySize.y;

		float scale[2];
		scale[0] = 2.0f / drawData->DisplaySize.x;
		scale[1] = 2.0f / drawData->DisplaySize.y;

		float translate[2];
		translate[0] = -1.0f - drawData->DisplayPos.x * scale[0];
		translate[1] = -1.0f - drawData->DisplayPos.y * scale[1];

		s_ScaleTranslateDescriptor.Scale = glm::vec2(scale[0], scale[1]);
		s_ScaleTranslateDescriptor.Translate = glm::vec2(translate[0], translate[1]);

		m_CameraBuffer->Update(&s_ScaleTranslateDescriptor, sizeof(ImGuiScaleTranslate), currentFrame);


		ImVec2 clipOffset = drawData->DisplayPos;
		ImVec2 clipScale = drawData->FramebufferScale;

		int fbWidth = (int)(drawData->DisplaySize.x * drawData->FramebufferScale.x);
		int fbHeight = (int)(drawData->DisplaySize.y * drawData->FramebufferScale.y);

		if (fbWidth == 0 || fbHeight == 0)
			return;

		auto& commandBuffer = Renderer::GetContext()->GetRendererData().GraphicsCommandBuffers[Renderer::GetContext()->GetCurrentFrame()];

		if (drawData->TotalVtxCount > 0)
		{
			size_t vertexSize = drawData->TotalVtxCount * sizeof(ImDrawVert);
			size_t indexSize = drawData->TotalIdxCount * sizeof(ImDrawIdx);

			std::vector<ImDrawVert> vertexList(drawData->TotalVtxCount);
			std::vector<ImDrawIdx> indexList(drawData->TotalIdxCount);

			uint32_t vertexLocation = 0;
			uint32_t indexLocation = 0;

			for (uint32_t i = 0; i < (uint32_t)drawData->CmdListsCount; i++)
			{

				memcpy(vertexList.data() + vertexLocation, drawData->CmdLists[i]->VtxBuffer.Data, (uint32_t)drawData->CmdLists[i]->VtxBuffer.Size * sizeof(ImDrawVert));
				memcpy(indexList.data() + indexLocation, drawData->CmdLists[i]->IdxBuffer.Data, (uint32_t)drawData->CmdLists[i]->IdxBuffer.Size * sizeof(ImDrawIdx));

				vertexLocation += drawData->CmdLists[i]->VtxBuffer.Size;
				indexLocation += drawData->CmdLists[i]->IdxBuffer.Size;
			}


			m_Renderer->SubmitBuffers(vertexList, indexList);

			m_Renderer->Bind();

			int globalVertexOffset = 0;
			int globalIndexOffset = 0;

			for (uint32_t listIndex = 0; listIndex < (uint32_t)drawData->CmdListsCount; listIndex++)
			{
				const ImDrawList* commandList = drawData->CmdLists[listIndex];

				for (uint32_t i = 0; i < (uint32_t)commandList->CmdBuffer.Size; i++)
				{
					const ImDrawCmd* command = &commandList->CmdBuffer[i];

					if (command->UserCallback != NULL)
					{
						if (command->UserCallback == ImDrawCallback_ResetRenderState)
						{
							// Set render state
						}
						else
						{
							command->UserCallback(commandList, command);
						}
					}
					else
					{
						ImVec2 clipMin((command->ClipRect.x - clipOffset.x) * clipScale.x, (command->ClipRect.y - clipOffset.y) * clipScale.y);
						ImVec2 clipMax((command->ClipRect.z - clipOffset.x) * clipScale.x, (command->ClipRect.w - clipOffset.y) * clipScale.y);

						if (clipMin.x < 0.0f) { clipMin.x = 0.0f; }
						if (clipMin.y < 0.0f) { clipMin.y = 0.0f; }
						if (clipMax.x > fbWidth) { clipMax.x = (float)fbWidth; }
						if (clipMax.y > fbHeight) { clipMax.y = (float)fbHeight; }

						if (clipMax.x <= clipMin.x || clipMax.y <= clipMin.y)
							continue;

						VkRect2D scissor;
						scissor.offset.x = (int32_t)(clipMin.x);
						scissor.offset.y = (int32_t)(clipMin.y);
						scissor.extent.width = (uint32_t)(clipMax.x - clipMin.x);
						scissor.extent.height = (uint32_t)(clipMax.y - clipMin.y);
						vkCmdSetScissor(commandBuffer->GetCommandBuffer(), 0, 1, &scissor);

						m_CameraDescriptor->Bind(commandBuffer, m_Renderer->GetPipelineLayout());
//						m_AtlasDescriptor->Bind(commandBuffer, m_Renderer->GetPipelineLayout());

						vkCmdBindDescriptorSets(commandBuffer->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_Renderer->GetPipelineLayout(), ImGuiTextureDescriptor, 1, (VkDescriptorSet*)&command->TextureId, 0, nullptr);

						vkCmdDrawIndexed(commandBuffer->GetCommandBuffer(), command->ElemCount, 1, command->IdxOffset + globalIndexOffset, command->VtxOffset + globalVertexOffset, 0);
					}
				}

				globalVertexOffset += commandList->VtxBuffer.Size;
				globalIndexOffset += commandList->IdxBuffer.Size;
			}

			VkRect2D scissor = { { 0, 0 }, { (uint32_t)fbWidth, (uint32_t)fbHeight } };
			vkCmdSetScissor(commandBuffer->GetCommandBuffer(), 0, 1, &scissor);
		}

		m_Renderer->EndScene();

	}

}
