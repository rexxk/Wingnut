#include "wingnut_pch.h"
#include "ImGuiRenderer.h"


#include "Assets/ShaderStore.h"

#include "Event/EventUtils.h"
#include "Event/WindowEvents.h"

#include "Platform/Vulkan/Buffer.h"
#include "Platform/Vulkan/Image.h"

#include "Renderer/Renderer.h"



namespace Wingnut
{


	struct ImGuiSceneData
	{
		Ref<Vulkan::Shader> Shader = nullptr;
		Ref<Vulkan::Pipeline> Pipeline = nullptr;

		std::unordered_map<UUID, std::pair<Ref<Vulkan::VertexBuffer>, Ref<Vulkan::IndexBuffer>>> DrawList;
		std::unordered_map<UUID, std::pair<Ref<Vulkan::VertexBuffer>, Ref<Vulkan::IndexBuffer>>> DrawCache;

	};

	static ImGuiSceneData s_ImGuiSceneData;


	VkIndexType DataSizeToIndexType(uint32_t size)
	{
		switch (size)
		{
			case 1: return VK_INDEX_TYPE_UINT8_EXT;
			case 2: return VK_INDEX_TYPE_UINT16;
			case 4: return VK_INDEX_TYPE_UINT32;
		}

		return VK_INDEX_TYPE_UINT32;
	}



	ImGuiRenderer::ImGuiRenderer(VkExtent2D extent)
		: m_Extent(extent)
	{
		Create();


		SubscribeToEvent<WindowResizedEvent>([&](WindowResizedEvent& event)
			{
				if (event.Width() == 0 || event.Height() == 0)
					return false;

				m_Extent.width = event.Width();
				m_Extent.height = event.Height();

				Release();
				Create();

				return false;
			});
	}

	ImGuiRenderer::~ImGuiRenderer()
	{
		void Release();
	}

	void ImGuiRenderer::Release()
	{

		s_ImGuiSceneData.DrawList.clear();
		s_ImGuiSceneData.DrawCache.clear();

		if (s_ImGuiSceneData.Pipeline != nullptr)
		{
			s_ImGuiSceneData.Pipeline->Release();
			s_ImGuiSceneData.Pipeline = nullptr;
		}

	}

	void ImGuiRenderer::Create()
	{
		LOG_CORE_TRACE("[ImGuiRenderer] Creating renderer");

		auto& rendererData = Renderer::GetContext()->GetRendererData();
		uint32_t framesInflight = Renderer::GetRendererSettings().FramesInFlight;

		s_ImGuiSceneData.Shader = ShaderStore::GetShader("ImGui");

		Vulkan::PipelineSpecification pipelineSpecification;
		pipelineSpecification.Extent = m_Extent;
		pipelineSpecification.PipelineShader = s_ImGuiSceneData.Shader;
		pipelineSpecification.RenderPass = rendererData.RenderPass;

		s_ImGuiSceneData.Pipeline = CreateRef<Vulkan::Pipeline>(rendererData.Device, pipelineSpecification);
	}

	void ImGuiRenderer::BeginScene(uint32_t currentFrame)
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();
		auto& commandBuffer = rendererData.GraphicsCommandBuffers[currentFrame];

		UpdateEntityCache();

		s_ImGuiSceneData.DrawList.clear();

		m_CurrentFrame = currentFrame;

		vkCmdBindPipeline(commandBuffer->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, s_ImGuiSceneData.Pipeline->GetPipeline());

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)m_Extent.width;
		viewport.height = (float)m_Extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer->GetCommandBuffer(), 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = m_Extent;
		vkCmdSetScissor(commandBuffer->GetCommandBuffer(), 0, 1, &scissor);
	}

	void ImGuiRenderer::EndScene()
	{

	}


	void ImGuiRenderer::Draw()
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();
		auto& commandBuffer = rendererData.GraphicsCommandBuffers[m_CurrentFrame];

		vkCmdBindPipeline(commandBuffer->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, s_ImGuiSceneData.Pipeline->GetPipeline());

		for (auto& entity : s_ImGuiSceneData.DrawList)
		{
			auto& [vertexBuffer, indexBuffer] = entity.second;

			VkBuffer vertexBuffers[] = { vertexBuffer->GetBuffer() };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer->GetCommandBuffer(), 0, 1, vertexBuffers, offsets);

			vkCmdBindIndexBuffer(commandBuffer->GetCommandBuffer(), indexBuffer->GetBuffer(), 0, DataSizeToIndexType(sizeof(ImDrawIdx)));

			s_ImGuiSceneData.Shader->BindDescriptorSets(commandBuffer->GetCommandBuffer(), s_ImGuiSceneData.Pipeline->GetLayout());

			vkCmdDrawIndexed(commandBuffer->GetCommandBuffer(), indexBuffer->IndexCount(), 1, 0, 0, 0);
		}

	}


	void ImGuiRenderer::UpdateDescriptor(uint32_t set, uint32_t binding, VkBuffer buffer, uint32_t bufferSize)
	{
		s_ImGuiSceneData.Shader->UpdateDescriptorSet(set, binding, buffer, bufferSize);
	}

	void ImGuiRenderer::UpdateDescriptor(uint32_t set, uint32_t binding, VkImageView imageView, VkSampler sampler)
	{
		s_ImGuiSceneData.Shader->UpdateDescriptorSet(set, binding, imageView, sampler);
	}


	void ImGuiRenderer::SubmitToDrawList(UUID entityID, const std::vector<ImDrawVert>& vertexList, const std::vector<ImDrawIdx>& indexList)
	{
		if (s_ImGuiSceneData.DrawCache.find(entityID) == s_ImGuiSceneData.DrawCache.end())
		{
			auto& device = Renderer::GetContext()->GetRendererData().Device;

			Ref<Vulkan::VertexBuffer> vertexBuffer = CreateRef<Vulkan::VertexBuffer>(device, vertexList.data(), (uint32_t)vertexList.size() * sizeof(ImDrawVert));
			Ref<Vulkan::IndexBuffer> indexBuffer = CreateRef<Vulkan::IndexBuffer>(device, indexList.data(), (uint32_t)indexList.size() * sizeof(ImDrawIdx), (uint32_t)indexList.size());

			s_ImGuiSceneData.DrawCache[entityID] = std::make_pair(vertexBuffer, indexBuffer);
		}

		s_ImGuiSceneData.DrawList[entityID] = s_ImGuiSceneData.DrawCache[entityID];
	}

	void ImGuiRenderer::UpdateEntityCache()
	{
		for (auto& entity : s_ImGuiSceneData.DrawCache)
		{
			if (std::find(s_ImGuiSceneData.DrawList.begin(), s_ImGuiSceneData.DrawList.end(), entity) == s_ImGuiSceneData.DrawList.end())
			{
				s_ImGuiSceneData.DrawCache.erase(s_ImGuiSceneData.DrawCache.find(entity.first));
			}
		}
	}



}
