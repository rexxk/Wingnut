#include "wingnut_pch.h"
#include "SceneRenderer.h"


#include "Assets/ShaderStore.h"

#include "Event/EventUtils.h"
#include "Event/WindowEvents.h"

#include "Platform/Vulkan/Buffer.h"
#include "Platform/Vulkan/CommandPool.h"
#include "Platform/Vulkan/CommandBuffer.h"
#include "Platform/Vulkan/Fence.h"
#include "Platform/Vulkan/Framebuffer.h"
#include "Platform/Vulkan/Image.h"
#include "Platform/Vulkan/RenderPass.h"
#include "Platform/Vulkan/Semaphore.h"

#include "Renderer/Renderer.h"


namespace Wingnut
{


	struct SceneData
	{
		Ref<Vulkan::CommandPool> GraphicsCommandPool = nullptr;

		std::vector<Ref<Vulkan::CommandBuffer>> GraphicsCommandBuffers;

		Ref<Vulkan::Shader> StaticSceneShader = nullptr;
		Ref<Vulkan::Shader> DynamicSceneShader = nullptr;
		Ref<Vulkan::Pipeline> StaticPipeline = nullptr;
		Ref<Vulkan::Pipeline> DynamicPipeline = nullptr;
		Ref<Vulkan::Framebuffer> Framebuffer = nullptr;
		
		Ref<Vulkan::RenderPass> RenderPass = nullptr;

		Ref<Vulkan::Image> DepthStencilImage = nullptr;

		std::vector<Ref<Vulkan::Fence>> InFlightFences;

		std::unordered_map<UUID, std::pair<Ref<Vulkan::VertexBuffer>, Ref<Vulkan::IndexBuffer>>> DrawList;
		std::unordered_map<UUID, std::pair<Ref<Vulkan::VertexBuffer>, Ref<Vulkan::IndexBuffer>>> DrawCache;

	};


	static SceneData s_SceneData;


	SceneRenderer::SceneRenderer(VkExtent2D extent)
		: m_Extent(extent)
	{
		Create();

		SubscribeToEvent<WindowResizedEvent>([&](WindowResizedEvent& event)
			{
				auto& rendererData = Renderer::GetContext()->GetRendererData();

				if (event.Width() == 0 || event.Height() == 0)
					return false;

				VkExtent2D extent = {};
				extent.width = event.Width();
				extent.height = event.Height();

				m_Extent = extent;

				vkDeviceWaitIdle(rendererData.Device->GetDevice());

				rendererData.Swapchain->Resize((VkSurfaceKHR)rendererData.Surface->GetSurface(), extent);

				s_SceneData.DepthStencilImage->Release();
				s_SceneData.DepthStencilImage = CreateRef<Vulkan::Image>(rendererData.Device, Vulkan::ImageType::DepthStencil, (uint32_t)extent.width, (uint32_t)extent.height,
					VK_FORMAT_D32_SFLOAT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_ASPECT_DEPTH_BIT);

				s_SceneData.Framebuffer->Release();
				s_SceneData.Framebuffer = CreateRef<Vulkan::Framebuffer>(rendererData.Device, rendererData.Swapchain, s_SceneData.RenderPass, s_SceneData.DepthStencilImage->GetImageView(), extent);

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

		for (auto& inFlightFence : s_SceneData.InFlightFences)
		{
			inFlightFence->Release();
		}

		for (auto& graphicsCommandBuffer : s_SceneData.GraphicsCommandBuffers)
		{
			graphicsCommandBuffer->Release();
		}

		if (s_SceneData.Framebuffer != nullptr)
		{
			s_SceneData.Framebuffer->Release();
		}

		if (s_SceneData.RenderPass != nullptr)
		{
			s_SceneData.RenderPass->Release();
		}

		if (s_SceneData.GraphicsCommandPool != nullptr)
		{
			s_SceneData.GraphicsCommandPool->Release();
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

		if (s_SceneData.DepthStencilImage != nullptr)
		{
			s_SceneData.DepthStencilImage->Release();
			s_SceneData.DepthStencilImage = nullptr;
		}
	}

	void SceneRenderer::Create()
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();
		uint32_t framesInflight = Renderer::GetRendererSettings().FramesInFlight;

		s_SceneData.DepthStencilImage = CreateRef<Vulkan::Image>(rendererData.Device, Vulkan::ImageType::DepthStencil, (uint32_t)m_Extent.width, (uint32_t)m_Extent.height,
			VK_FORMAT_D32_SFLOAT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_ASPECT_DEPTH_BIT);

		s_SceneData.RenderPass = CreateRef<Vulkan::RenderPass>(rendererData.Device, rendererData.Device->GetDeviceProperties().SurfaceFormat.format);

		s_SceneData.StaticSceneShader = ShaderStore::GetShader("basic");

		Vulkan::PipelineSpecification pipelineSpecification;
		pipelineSpecification.Extent = m_Extent;
		pipelineSpecification.PipelineShader = s_SceneData.StaticSceneShader;
		pipelineSpecification.RenderPass = s_SceneData.RenderPass;

		s_SceneData.StaticPipeline = CreateRef<Vulkan::Pipeline>(rendererData.Device, pipelineSpecification);
		s_SceneData.GraphicsCommandPool = CreateRef<Vulkan::CommandPool>(rendererData.Device, Vulkan::CommandPoolType::Graphics);

		s_SceneData.Framebuffer = CreateRef<Vulkan::Framebuffer>(rendererData.Device, rendererData.Swapchain, s_SceneData.RenderPass, s_SceneData.DepthStencilImage->GetImageView(), rendererData.Device->GetDeviceProperties().SurfaceCapabilities.currentExtent);


		for (uint32_t i = 0; i < framesInflight; i++)
		{
			Ref<Vulkan::CommandBuffer> newGraphicsCommandBuffer = CreateRef<Vulkan::CommandBuffer>(rendererData.Device, s_SceneData.GraphicsCommandPool);
			s_SceneData.GraphicsCommandBuffers.emplace_back(newGraphicsCommandBuffer);

			Ref<Vulkan::Fence> newInFlightFence = CreateRef<Vulkan::Fence>(rendererData.Device);
			s_SceneData.InFlightFences.emplace_back(newInFlightFence);
		}
	}

	void SceneRenderer::BeginScene(uint32_t currentFrame)
	{
		UpdateEntityCache();

		s_SceneData.DrawList.clear();

		m_CurrentFrame = currentFrame;

		auto& commandBuffer = s_SceneData.GraphicsCommandBuffers[currentFrame];

		s_SceneData.InFlightFences[currentFrame]->Wait(UINT64_MAX);
		s_SceneData.InFlightFences[currentFrame]->Reset();

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(commandBuffer->GetCommandBuffer(), &beginInfo) != VK_SUCCESS)
		{
			LOG_CORE_ERROR("[Renderer] Unable to begin command buffer recording");
			return;
		}

		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = s_SceneData.RenderPass->GetRenderPass();
		renderPassBeginInfo.framebuffer = s_SceneData.Framebuffer->GetNextFramebuffer();
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = m_Extent;

		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = { {0.2f, 0.3f, 0.45f} };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassBeginInfo.clearValueCount = (uint32_t)clearValues.size();
		renderPassBeginInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer->GetCommandBuffer(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

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
		auto& commandBuffer = s_SceneData.GraphicsCommandBuffers[m_CurrentFrame];

		vkCmdEndRenderPass(commandBuffer->GetCommandBuffer());

		if (vkEndCommandBuffer(commandBuffer->GetCommandBuffer()) != VK_SUCCESS)
		{
			LOG_CORE_ERROR("[Renderer] Unable to end command buffer recording");
			return;
		}
	}


	void SceneRenderer::Draw()
	{
		auto& commandBuffer = s_SceneData.GraphicsCommandBuffers[m_CurrentFrame];

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

	void SceneRenderer::SubmitQueue()
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();

//		uint32_t imageIndex = 0;
//		vkAcquireNextImageKHR(rendererData.Device->GetDevice(), rendererData.Swapchain->GetSwapchain(), UINT64_MAX, rendererData.ImageAvailableSemaphores[m_CurrentFrame]->GetSemaphore(), VK_NULL_HANDLE, &imageIndex);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { rendererData.ImageAvailableSemaphores[m_CurrentFrame]->GetSemaphore() };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		VkCommandBuffer commandBuffers[] = { s_SceneData.GraphicsCommandBuffers[m_CurrentFrame]->GetCommandBuffer() };

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = commandBuffers;

		VkSemaphore signalSemaphores[] = { rendererData.RenderFinishedSemaphores[m_CurrentFrame]->GetSemaphore() };

		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(rendererData.Device->GetQueue(Vulkan::QueueType::Graphics), 1, &submitInfo, s_SceneData.InFlightFences[m_CurrentFrame]->GetFence()))
		{
			LOG_CORE_ERROR("[Renderer] Unable to submit queue");
		}

	}

}
