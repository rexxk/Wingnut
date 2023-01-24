#include "wingnut_pch.h"
#include "ImGuiRenderer.h"


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


	struct ImGuiSceneData
	{
		Ref<Vulkan::CommandPool> CommandPool = nullptr;

		std::vector<Ref<Vulkan::CommandBuffer>> CommandBuffers;

		Ref<Vulkan::Shader> Shader = nullptr;
		Ref<Vulkan::Pipeline> Pipeline = nullptr;
		Ref<Vulkan::Framebuffer> Framebuffer = nullptr;

		Ref<Vulkan::RenderPass> RenderPass = nullptr;

		Ref<Vulkan::Image> DepthStencilImage = nullptr;

		std::vector<Ref<Vulkan::Fence>> InFlightFences;

		std::unordered_map<UUID, std::pair<Ref<Vulkan::VertexBuffer>, Ref<Vulkan::IndexBuffer>>> DrawList;
		std::unordered_map<UUID, std::pair<Ref<Vulkan::VertexBuffer>, Ref<Vulkan::IndexBuffer>>> DrawCache;

	};


	static ImGuiSceneData s_ImGuiSceneData;


	ImGuiRenderer::ImGuiRenderer(VkExtent2D extent)
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

				s_ImGuiSceneData.DepthStencilImage->Release();
				s_ImGuiSceneData.DepthStencilImage = CreateRef<Vulkan::Image>(rendererData.Device, Vulkan::ImageType::DepthStencil, (uint32_t)extent.width, (uint32_t)extent.height,
					VK_FORMAT_D32_SFLOAT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_ASPECT_DEPTH_BIT);

				s_ImGuiSceneData.Framebuffer->Release();
				s_ImGuiSceneData.Framebuffer = CreateRef<Vulkan::Framebuffer>(rendererData.Device, rendererData.Swapchain, s_ImGuiSceneData.RenderPass, s_ImGuiSceneData.DepthStencilImage->GetImageView(), extent);

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

		for (auto& inFlightFence : s_ImGuiSceneData.InFlightFences)
		{
			inFlightFence->Release();
		}

		for (auto& graphicsCommandBuffer : s_ImGuiSceneData.CommandBuffers)
		{
			graphicsCommandBuffer->Release();
		}

		if (s_ImGuiSceneData.Framebuffer != nullptr)
		{
			s_ImGuiSceneData.Framebuffer->Release();
		}

		if (s_ImGuiSceneData.RenderPass != nullptr)
		{
			s_ImGuiSceneData.RenderPass->Release();
		}

		if (s_ImGuiSceneData.CommandPool != nullptr)
		{
			s_ImGuiSceneData.CommandPool->Release();
		}

		if (s_ImGuiSceneData.Pipeline != nullptr)
		{
			s_ImGuiSceneData.Pipeline->Release();
			s_ImGuiSceneData.Pipeline = nullptr;
		}

		if (s_ImGuiSceneData.DepthStencilImage != nullptr)
		{
			s_ImGuiSceneData.DepthStencilImage->Release();
			s_ImGuiSceneData.DepthStencilImage = nullptr;
		}
	}

	void ImGuiRenderer::Create()
	{
		LOG_CORE_TRACE("[ImGuiRenderer] Creating renderer");

		auto& rendererData = Renderer::GetContext()->GetRendererData();
		uint32_t framesInflight = Renderer::GetRendererSettings().FramesInFlight;

		s_ImGuiSceneData.DepthStencilImage = CreateRef<Vulkan::Image>(rendererData.Device, Vulkan::ImageType::DepthStencil, (uint32_t)m_Extent.width, (uint32_t)m_Extent.height,
			VK_FORMAT_D32_SFLOAT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_ASPECT_DEPTH_BIT);

		s_ImGuiSceneData.RenderPass = CreateRef<Vulkan::RenderPass>(rendererData.Device, rendererData.Device->GetDeviceProperties().SurfaceFormat.format);

		s_ImGuiSceneData.Shader = ShaderStore::GetShader("ImGui");

		Vulkan::PipelineSpecification pipelineSpecification;
		pipelineSpecification.Extent = m_Extent;
		pipelineSpecification.PipelineShader = s_ImGuiSceneData.Shader;
		pipelineSpecification.RenderPass = s_ImGuiSceneData.RenderPass;

		s_ImGuiSceneData.Pipeline = CreateRef<Vulkan::Pipeline>(rendererData.Device, pipelineSpecification);
		s_ImGuiSceneData.CommandPool = CreateRef<Vulkan::CommandPool>(rendererData.Device, Vulkan::CommandPoolType::Graphics);

		s_ImGuiSceneData.Framebuffer = CreateRef<Vulkan::Framebuffer>(rendererData.Device, rendererData.Swapchain, s_ImGuiSceneData.RenderPass, s_ImGuiSceneData.DepthStencilImage->GetImageView(), rendererData.Device->GetDeviceProperties().SurfaceCapabilities.currentExtent);


		for (uint32_t i = 0; i < framesInflight; i++)
		{
			Ref<Vulkan::CommandBuffer> newGraphicsCommandBuffer = CreateRef<Vulkan::CommandBuffer>(rendererData.Device, s_ImGuiSceneData.CommandPool);
			s_ImGuiSceneData.CommandBuffers.emplace_back(newGraphicsCommandBuffer);

			Ref<Vulkan::Fence> newInFlightFence = CreateRef<Vulkan::Fence>(rendererData.Device);
			s_ImGuiSceneData.InFlightFences.emplace_back(newInFlightFence);
		}
	}

	void ImGuiRenderer::BeginScene(uint32_t currentFrame)
	{
		UpdateEntityCache();

		s_ImGuiSceneData.DrawList.clear();

		m_CurrentFrame = currentFrame;

		auto& commandBuffer = s_ImGuiSceneData.CommandBuffers[currentFrame];

		s_ImGuiSceneData.InFlightFences[currentFrame]->Wait(UINT64_MAX);
		s_ImGuiSceneData.InFlightFences[currentFrame]->Reset();

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(commandBuffer->GetCommandBuffer(), &beginInfo) != VK_SUCCESS)
		{
			LOG_CORE_ERROR("[ImGuiRenderer] Unable to begin command buffer recording");
			return;
		}

		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = s_ImGuiSceneData.RenderPass->GetRenderPass();
		renderPassBeginInfo.framebuffer = s_ImGuiSceneData.Framebuffer->GetNextFramebuffer();
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = m_Extent;

		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = { {0.2f, 0.3f, 0.45f} };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassBeginInfo.clearValueCount = (uint32_t)clearValues.size();
		renderPassBeginInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer->GetCommandBuffer(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

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
		auto& commandBuffer = s_ImGuiSceneData.CommandBuffers[m_CurrentFrame];

		vkCmdEndRenderPass(commandBuffer->GetCommandBuffer());

		if (vkEndCommandBuffer(commandBuffer->GetCommandBuffer()) != VK_SUCCESS)
		{
			LOG_CORE_ERROR("[ImGuiRenderer] Unable to end command buffer recording");
			return;
		}
	}


	void ImGuiRenderer::Draw()
	{
		auto& commandBuffer = s_ImGuiSceneData.CommandBuffers[m_CurrentFrame];

		vkCmdBindPipeline(commandBuffer->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, s_ImGuiSceneData.Pipeline->GetPipeline());

		for (auto& entity : s_ImGuiSceneData.DrawList)
		{
			auto& [vertexBuffer, indexBuffer] = entity.second;

			VkBuffer vertexBuffers[] = { vertexBuffer->GetBuffer() };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer->GetCommandBuffer(), 0, 1, vertexBuffers, offsets);

			vkCmdBindIndexBuffer(commandBuffer->GetCommandBuffer(), indexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

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


	void ImGuiRenderer::SubmitToDrawList(UUID entityID, const std::vector<Vertex>& vertexList, const std::vector<uint32_t>& indexList)
	{
		if (s_ImGuiSceneData.DrawCache.find(entityID) == s_ImGuiSceneData.DrawCache.end())
		{
			auto& device = Renderer::GetContext()->GetRendererData().Device;

			Ref<Vulkan::VertexBuffer> vertexBuffer = CreateRef<Vulkan::VertexBuffer>(device, vertexList);
			Ref<Vulkan::IndexBuffer> indexBuffer = CreateRef<Vulkan::IndexBuffer>(device, indexList);

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

	void ImGuiRenderer::SubmitQueue()
	{
		auto& rendererData = Renderer::GetContext()->GetRendererData();

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { rendererData.ImageAvailableSemaphores[m_CurrentFrame]->GetSemaphore() };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		VkCommandBuffer commandBuffers[] = { s_ImGuiSceneData.CommandBuffers[m_CurrentFrame]->GetCommandBuffer() };

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = commandBuffers;

		VkSemaphore signalSemaphores[] = { rendererData.RenderFinishedSemaphores[m_CurrentFrame]->GetSemaphore() };

		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(rendererData.Device->GetQueue(Vulkan::QueueType::Graphics), 1, &submitInfo, s_ImGuiSceneData.InFlightFences[m_CurrentFrame]->GetFence()))
		{
			LOG_CORE_ERROR("[ImGuiRenderer] Unable to submit queue");
		}

	}

}
