#include "wingnut_pch.h"
#include "SceneRenderer.h"


#include "Assets/ShaderStore.h"

#include "Event/EventUtils.h"
#include "Event/WindowEvents.h"

#include "Platform/Vulkan/CommandPool.h""
#include "Platform/Vulkan/CommandBuffer.h""
#include "Platform/Vulkan/Fence.h"
#include "Platform/Vulkan/Framebuffer.h"
#include "Platform/Vulkan/RenderPass.h"
#include "Platform/Vulkan/Semaphore.h"

#include "Renderer/Renderer.h"


namespace Wingnut
{


	struct SceneData
	{
		std::vector<Ref<Vulkan::CommandBuffer>> GraphicsCommandBuffers;
		Ref<Vulkan::CommandPool> GraphicsCommandPool = nullptr;

		Ref<Vulkan::Shader> StaticSceneShader = nullptr;
		Ref<Vulkan::Pipeline> GraphicsPipeline = nullptr;
		Ref<Vulkan::Framebuffer> Framebuffer = nullptr;
		
		Ref<Vulkan::RenderPass> RenderPass = nullptr;


		std::vector<Ref<Vulkan::Fence>> InFlightFences;
		std::vector<Ref<Vulkan::Semaphore>> ImageAvailableSemaphores;
		std::vector<Ref<Vulkan::Semaphore>> RenderFinishedSemaphores;
	};


	static SceneData s_SceneData;


	SceneRenderer::SceneRenderer(VkExtent2D extent)
		: m_Extent(extent)
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();

		uint32_t framesInflight = Renderer::GetRendererSettings().FramesInFlight;


		s_SceneData.RenderPass = CreateRef<Vulkan::RenderPass>(rendererData.Device, rendererData.Device->GetDeviceProperties().SurfaceFormat.format);

		s_SceneData.StaticSceneShader = ShaderStore::GetShader("basic");

		Vulkan::PipelineSpecification pipelineSpecification;
		pipelineSpecification.Extent = extent;
		pipelineSpecification.PipelineShader = s_SceneData.StaticSceneShader;
		pipelineSpecification.RenderPass = s_SceneData.RenderPass;

		s_SceneData.GraphicsPipeline = CreateRef<Vulkan::Pipeline>(rendererData.Device, pipelineSpecification);
		s_SceneData.GraphicsCommandPool = CreateRef<Vulkan::CommandPool>(rendererData.Device, Vulkan::CommandPoolType::Graphics);

		s_SceneData.Framebuffer = CreateRef<Vulkan::Framebuffer>(rendererData.Device, rendererData.Swapchain, s_SceneData.RenderPass, rendererData.Device->GetDeviceProperties().SurfaceCapabilities.currentExtent);


		for (uint32_t i = 0; i < framesInflight; i++)
		{
			Ref<Vulkan::CommandBuffer> newGraphicsCommandBuffer = CreateRef<Vulkan::CommandBuffer>(rendererData.Device, s_SceneData.GraphicsCommandPool);
			s_SceneData.GraphicsCommandBuffers.emplace_back(newGraphicsCommandBuffer);

			Ref<Vulkan::Fence> newInFlightFence = CreateRef<Vulkan::Fence>(rendererData.Device);
			s_SceneData.InFlightFences.emplace_back(newInFlightFence);

			Ref<Vulkan::Semaphore> newImageAvailableSemaphore = CreateRef<Vulkan::Semaphore>(rendererData.Device);
			s_SceneData.ImageAvailableSemaphores.emplace_back(newImageAvailableSemaphore);

			Ref<Vulkan::Semaphore> newRenderFinishedSemaphore = CreateRef<Vulkan::Semaphore>(rendererData.Device);
			s_SceneData.RenderFinishedSemaphores.emplace_back(newRenderFinishedSemaphore);
		}

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

				s_SceneData.Framebuffer->Release();
				s_SceneData.Framebuffer = CreateRef<Vulkan::Framebuffer>(rendererData.Device, rendererData.Swapchain, s_SceneData.RenderPass, extent);

				return false;
			});

	}

	SceneRenderer::~SceneRenderer()
	{
		void Release();
	}

	void SceneRenderer::Release()
	{

		for (auto& inFlightFence : s_SceneData.InFlightFences)
		{
			inFlightFence->Release();
		}

		for (auto& renderFinishedSemaphore : s_SceneData.RenderFinishedSemaphores)
		{
			renderFinishedSemaphore->Release();
		}

		for (auto& imageAvailableSemaphore : s_SceneData.ImageAvailableSemaphores)
		{
			imageAvailableSemaphore->Release();
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

		if (s_SceneData.GraphicsPipeline != nullptr)
		{
			s_SceneData.GraphicsPipeline->Release();
			s_SceneData.GraphicsPipeline = nullptr;
		}

	}


	void SceneRenderer::BeginScene(uint32_t currentFrame)
	{
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

		VkClearValue clearColor = { {{ 0.2f, 0.3f, 0.45f }} };
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffer->GetCommandBuffer(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffer->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, s_SceneData.GraphicsPipeline->GetPipeline());

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

	void SceneRenderer::Present()
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();

		uint32_t framesInFlight = Renderer::GetRendererSettings().FramesInFlight;

		uint32_t imageIndex = 0;
		vkAcquireNextImageKHR(rendererData.Device->GetDevice(), rendererData.Swapchain->GetSwapchain(), UINT64_MAX, s_SceneData.ImageAvailableSemaphores[m_CurrentFrame]->GetSemaphore(), VK_NULL_HANDLE, &imageIndex);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { s_SceneData.ImageAvailableSemaphores[m_CurrentFrame]->GetSemaphore() };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		VkCommandBuffer commandBuffers[] = { s_SceneData.GraphicsCommandBuffers[m_CurrentFrame]->GetCommandBuffer() };

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = commandBuffers;

		VkSemaphore signalSemaphores[] = { s_SceneData.RenderFinishedSemaphores[m_CurrentFrame]->GetSemaphore() };

		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(rendererData.Device->GetQueue(Vulkan::QueueType::Graphics), 1, &submitInfo, s_SceneData.InFlightFences[m_CurrentFrame]->GetFence()))
		{
			LOG_CORE_ERROR("[Renderer] Unable to submit queue");
		}

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapchains[] = { rendererData.Swapchain->GetSwapchain() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapchains;
		presentInfo.pImageIndices = &imageIndex;


		if (vkQueuePresentKHR(rendererData.Device->GetQueue(Vulkan::QueueType::Graphics), &presentInfo) != VK_SUCCESS)
		{
			//			LOG_CORE_ERROR("[Renderer] Failed to present queue");
			return;
		}

		m_CurrentFrame = (m_CurrentFrame++) & framesInFlight;

	}


	void SceneRenderer::Draw(Ref<Vulkan::VertexBuffer> vertexBuffer, Ref<Vulkan::IndexBuffer> indexBuffer)
	{
		auto& commandBuffer = s_SceneData.GraphicsCommandBuffers[m_CurrentFrame];

		vkCmdBindPipeline(commandBuffer->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, s_SceneData.GraphicsPipeline->GetPipeline());

		VkBuffer vertexBuffers[] = { vertexBuffer->GetBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer->GetCommandBuffer(), 0, 1, vertexBuffers, offsets);

		vkCmdBindIndexBuffer(commandBuffer->GetCommandBuffer(), indexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

		s_SceneData.StaticSceneShader->BindDescriptorSets(commandBuffer->GetCommandBuffer(), s_SceneData.GraphicsPipeline->GetLayout());

		vkCmdDrawIndexed(commandBuffer->GetCommandBuffer(), indexBuffer->IndexCount(), 1, 0, 0, 0);

	}


	void SceneRenderer::UpdateDescriptor(uint32_t set, uint32_t binding, VkBuffer buffer, uint32_t bufferSize)
	{
		s_SceneData.StaticSceneShader->UpdateDescriptorSet(set, binding, buffer, bufferSize);
	}

	void SceneRenderer::UpdateDescriptor(uint32_t set, uint32_t binding, VkImageView imageView, VkSampler sampler)
	{
		s_SceneData.StaticSceneShader->UpdateDescriptorSet(set, binding, imageView, sampler);
	}

}
