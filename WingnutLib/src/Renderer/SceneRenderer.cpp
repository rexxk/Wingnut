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

		std::vector<Ref<Vulkan::Descriptor>> DescriptorList;
	};


	static SceneData s_SceneData;


	Ref<SceneRenderer> SceneRenderer::Create(VkExtent2D extent)
	{
		return CreateRef<SceneRenderer>(extent);
	}


	SceneRenderer::SceneRenderer(VkExtent2D extent)
		: m_Extent(extent)
	{
		CreateRenderer();


		SubscribeToEvent<WindowResizedEvent>([&](WindowResizedEvent& event)
			{
				if (event.Width() == 0 || event.Height() == 0)
					return false;

				m_Extent.width = event.Width();
				m_Extent.height = event.Height();

				Release();
				CreateRenderer();

				return false;
			});
	}

	SceneRenderer::~SceneRenderer()
	{
		void Release();
	}

	void SceneRenderer::Release()
	{

		s_SceneData.DrawList.clear();
		s_SceneData.DrawCache.clear();

		if (m_RenderImage != nullptr)
		{
			m_RenderImage->Release();
		}

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

	void SceneRenderer::CreateRenderer()
	{
		LOG_CORE_TRACE("[SceneRenderer] Creating renderer");

		auto& rendererData = Renderer::GetContext()->GetRendererData();
		uint32_t framesInflight = Renderer::GetRendererSettings().FramesInFlight;

		s_SceneData.StaticSceneShader = ShaderStore::GetShader("basic");

		Vulkan::PipelineSpecification pipelineSpecification;
		pipelineSpecification.Extent = m_Extent;
		pipelineSpecification.PipelineShader = s_SceneData.StaticSceneShader;
		pipelineSpecification.CullMode = Vulkan::CullMode::Back;
		pipelineSpecification.CullingDirection = Vulkan::CullingDirection::Clockwise;
		pipelineSpecification.RenderPass = rendererData.SceneRenderPass;
		pipelineSpecification.DepthTestEnable = true;
		pipelineSpecification.DepthWriteEnable = true;

		s_SceneData.StaticPipeline = Vulkan::Pipeline::Create(rendererData.Device, pipelineSpecification);


		m_RenderImage = Vulkan::Image::Create(rendererData.Device, Vulkan::ImageType::Texture2D, m_Extent.width, m_Extent.height, VK_FORMAT_R8G8B8A8_UNORM,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);

//		m_RenderImage->TransitionLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	}

	void SceneRenderer::BeginScene(uint32_t currentFrame)
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();
		auto& commandBuffer = rendererData.GraphicsCommandBuffers[currentFrame];

		s_SceneData.DescriptorList.clear();

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
		auto& rendererData = Renderer::GetContext()->GetRendererData();
		auto& commandBuffer = rendererData.GraphicsCommandBuffers[m_CurrentFrame];

		vkCmdEndRenderPass(commandBuffer->GetCommandBuffer());

//		m_RenderImage->TransitionLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		
/*
		VkImageBlit region = { };
		region.srcOffsets[0] = { 0, 0, 0 };
		region.srcOffsets[1] = { (int)m_Extent.width, (int)m_Extent.height, 1 };
		region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.srcSubresource.layerCount = 1;

		region.dstOffsets[0] = { 0, 0, 0 };
		region.dstOffsets[1] = { (int)m_Extent.width, (int)m_Extent.height, 1 };
		region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.dstSubresource.layerCount = 1;

//		rendererData.SceneTexture->TransitionLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		rendererData.SceneTexture->TransitionLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

		vkCmdBlitImage(commandBuffer->GetCommandBuffer(), rendererData.SceneTexture->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_RenderImage->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region, VK_FILTER_LINEAR);
*/

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

			for (auto& descriptor : s_SceneData.DescriptorList)
			{
				descriptor->Bind(commandBuffer, s_SceneData.StaticPipeline->GetLayout());
			}

			vkCmdDrawIndexed(commandBuffer->GetCommandBuffer(), indexBuffer->IndexCount(), 1, 0, 0, 0);
		}

	}

	void SceneRenderer::SubmitDescriptor(Ref<Vulkan::Descriptor> descriptor)
	{
		s_SceneData.DescriptorList.emplace_back(descriptor);
	}

	void SceneRenderer::SubmitToDrawList(UUID entityID, const std::vector<Vertex>& vertexList, const std::vector<uint32_t>& indexList)
	{
		if (s_SceneData.DrawCache.find(entityID) == s_SceneData.DrawCache.end())
		{
			auto& device = Renderer::GetContext()->GetRendererData().Device;

			Ref<Vulkan::VertexBuffer> vertexBuffer = Vulkan::VertexBuffer::Create(device, vertexList.data(), (uint32_t)vertexList.size() * sizeof(Vertex));
			Ref<Vulkan::IndexBuffer> indexBuffer = Vulkan::IndexBuffer::Create(device, indexList.data(), (uint32_t)indexList.size() * sizeof(uint32_t), (uint32_t)indexList.size());

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
