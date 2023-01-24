#include "wingnut_pch.h"
#include "SceneRenderer.h"


#include "Assets/ShaderStore.h"

#include "Event/EventUtils.h"
#include "Event/WindowEvents.h"

#include "Platform/Vulkan/Buffer.h"
#include "Platform/Vulkan/Image.h"
#include "Platform/Vulkan/RenderPass.h"

#include "Renderer/Renderer.h"


namespace Wingnut
{


	struct SceneData
	{
		Ref<Vulkan::Shader> StaticSceneShader = nullptr;
		Ref<Vulkan::Shader> DynamicSceneShader = nullptr;
		Ref<Vulkan::Pipeline> StaticPipeline = nullptr;
		Ref<Vulkan::Pipeline> DynamicPipeline = nullptr;

		std::unordered_map<UUID, std::pair<Ref<Vulkan::VertexBuffer>, Ref<Vulkan::IndexBuffer>>> DrawList;
		std::unordered_map<UUID, std::pair<Ref<Vulkan::VertexBuffer>, Ref<Vulkan::IndexBuffer>>> DrawCache;
	};


	static SceneData s_SceneData;


	SceneRenderer::SceneRenderer(VkExtent2D extent)
		: m_Extent(extent)
	{
		Create();
	}

	SceneRenderer::~SceneRenderer()
	{
		void Release();
	}

	void SceneRenderer::Release()
	{

		s_SceneData.DrawList.clear();
		s_SceneData.DrawCache.clear();


		if (s_SceneData.StaticPipeline != nullptr)
		{
			s_SceneData.StaticPipeline->Release();
			s_SceneData.StaticPipeline = nullptr;
		}

		if (s_SceneData.DynamicPipeline != nullptr)
		{
			s_SceneData.DynamicPipeline->Release();
			s_SceneData.DynamicPipeline = nullptr;
		}

	}

	void SceneRenderer::Create()
	{
		LOG_CORE_TRACE("[SceneRenderer] Creating renderer");

		auto& rendererData = Renderer::GetContext()->GetRendererData();
		uint32_t framesInflight = Renderer::GetRendererSettings().FramesInFlight;

		s_SceneData.StaticSceneShader = ShaderStore::GetShader("basic");

		Vulkan::PipelineSpecification pipelineSpecification;
		pipelineSpecification.Extent = m_Extent;
		pipelineSpecification.PipelineShader = s_SceneData.StaticSceneShader;
		pipelineSpecification.RenderPass = rendererData.RenderPass;

		s_SceneData.StaticPipeline = CreateRef<Vulkan::Pipeline>(rendererData.Device, pipelineSpecification);
	}

	void SceneRenderer::BeginScene(uint32_t currentFrame)
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();
		auto& commandBuffer = rendererData.GraphicsCommandBuffers[currentFrame];

		UpdateEntityCache();

		s_SceneData.DrawList.clear();

		m_CurrentFrame = currentFrame;

		vkCmdBindPipeline(commandBuffer->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, s_SceneData.StaticPipeline->GetPipeline());

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

	void SceneRenderer::EndScene()
	{

	}


	void SceneRenderer::Draw()
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();
		auto& commandBuffer = rendererData.GraphicsCommandBuffers[m_CurrentFrame];

		vkCmdBindPipeline(commandBuffer->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, s_SceneData.StaticPipeline->GetPipeline());

		for (auto& entity : s_SceneData.DrawList)
		{
			auto [vertexBuffer, indexBuffer] = entity.second;
	
			VkBuffer vertexBuffers[] = { vertexBuffer->GetBuffer() };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer->GetCommandBuffer(), 0, 1, vertexBuffers, offsets);

			vkCmdBindIndexBuffer(commandBuffer->GetCommandBuffer(), indexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

			s_SceneData.StaticSceneShader->BindDescriptorSets(commandBuffer->GetCommandBuffer(), s_SceneData.StaticPipeline->GetLayout());

			vkCmdDrawIndexed(commandBuffer->GetCommandBuffer(), indexBuffer->IndexCount(), 1, 0, 0, 0);
		}

	}


	void SceneRenderer::UpdateDescriptor(uint32_t set, uint32_t binding, VkBuffer buffer, uint32_t bufferSize)
	{
		s_SceneData.StaticSceneShader->UpdateDescriptorSet(set, binding, buffer, bufferSize);
	}

	void SceneRenderer::UpdateDescriptor(uint32_t set, uint32_t binding, VkImageView imageView, VkSampler sampler)
	{
		s_SceneData.StaticSceneShader->UpdateDescriptorSet(set, binding, imageView, sampler);
	}


	void SceneRenderer::SubmitToDrawList(UUID entityID, const std::vector<Vertex>& vertexList, const std::vector<uint32_t>& indexList)
	{
		if (s_SceneData.DrawCache.find(entityID) == s_SceneData.DrawCache.end())
		{
			auto& device = Renderer::GetContext()->GetRendererData().Device;

			Ref<Vulkan::VertexBuffer> vertexBuffer = CreateRef<Vulkan::VertexBuffer>(device, vertexList);
			Ref<Vulkan::IndexBuffer> indexBuffer = CreateRef<Vulkan::IndexBuffer>(device, indexList);

			s_SceneData.DrawCache[entityID] = std::make_pair(vertexBuffer, indexBuffer);
		}

		s_SceneData.DrawList[entityID] = s_SceneData.DrawCache[entityID];
	}

	void SceneRenderer::UpdateEntityCache()
	{
		for (auto& entity : s_SceneData.DrawCache)
		{
			if (std::find(s_SceneData.DrawList.begin(), s_SceneData.DrawList.end(), entity) == s_SceneData.DrawList.end())
			{
				s_SceneData.DrawCache.erase(s_SceneData.DrawCache.find(entity.first));
			}
		}
	}

}
