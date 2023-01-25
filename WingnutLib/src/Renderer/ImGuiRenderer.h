#pragma once


#include "Platform/Vulkan/Buffer.h"
#include "Platform/Vulkan/Pipeline.h"
#include "Platform/Vulkan/RenderPass.h"
#include "Platform/Vulkan/Shader.h"

#include <imgui.h>


namespace Wingnut
{






	class ImGuiRenderer
	{
	public:
		ImGuiRenderer(VkExtent2D extent);
		~ImGuiRenderer();

		void Release();

		void BeginScene(uint32_t currentFrame);
		void EndScene();

		void Bind();


		void UpdateDescriptor(uint32_t set, uint32_t binding, VkBuffer buffer, uint32_t bufferSize);
		void UpdateDescriptor(uint32_t set, uint32_t binding, VkImageView imageView, VkSampler sampler);

		void SubmitBuffers(const std::vector<ImDrawVert>& vertexList, const std::vector<ImDrawIdx>& indexList);

	private:
		void Create();
		void UpdateEntityCache();

	private:

		VkExtent2D m_Extent;

		uint32_t m_CurrentFrame;

		Ref<Vulkan::VertexBuffer> m_VertexBuffer = nullptr;
		Ref<Vulkan::IndexBuffer> m_IndexBuffer = nullptr;
	};


}
