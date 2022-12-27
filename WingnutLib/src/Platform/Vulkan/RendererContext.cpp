#include "wingnut_pch.h"
#include "RendererContext.h"

#include "ShaderCompiler.h"

#include "Assets/ShaderStore.h"

#include "Event/EventUtils.h"
#include "Event/WindowEvents.h"


namespace Wingnut
{

	namespace Vulkan
	{

		static RendererData s_VulkanData;


		VulkanContext::VulkanContext(void* windowHandle)
		{
			Create(windowHandle);

			m_CurrentExtent = s_VulkanData.Device->GetDeviceProperties().SurfaceCapabilities.currentExtent;

			SubscribeToEvent<WindowResizedEvent>([&](WindowResizedEvent& event)
				{
					if (event.Width() == 0 || event.Height() == 0)
						return false;

					VkExtent2D extent;
					extent.width = event.Width();
					extent.height = event.Height();

					m_CurrentExtent = extent;

					vkDeviceWaitIdle(s_VulkanData.Device->GetDevice());

					s_VulkanData.Swapchain->Resize((VkSurfaceKHR)s_VulkanData.Surface->GetSurface(), extent);

					s_VulkanData.Framebuffer->Release();
					s_VulkanData.Framebuffer = CreateRef<Framebuffer>(s_VulkanData.Device, s_VulkanData.Swapchain, s_VulkanData.RenderPass, extent);


					return false;
				});
		}

		VulkanContext::~VulkanContext()
		{

		}

		void VulkanContext::ReleaseAll()
		{
			if (s_VulkanData.Device->GetDevice() != nullptr)
			{
				s_VulkanData.Device->WaitForIdle();
			}

			for (auto& inFlightFence : s_VulkanData.InFlightFences)
			{
				inFlightFence->Release();
			}

			for (auto& renderFinishedSemaphore : s_VulkanData.RenderFinishedSemaphores)
			{
				renderFinishedSemaphore->Release();
			}

			for (auto& imageAvailableSemaphore : s_VulkanData.ImageAvailableSemaphores)
			{
				imageAvailableSemaphore->Release();
			}

			for (auto& graphicsCommandBuffer : s_VulkanData.GraphicsCommandBuffers)
			{
				graphicsCommandBuffer->Release();
			}

			ShaderStore::Release();

			if (s_VulkanData.Pipeline != nullptr)
			{
				s_VulkanData.Pipeline->Release();
			}

			if (s_VulkanData.Framebuffer != nullptr)
			{
				s_VulkanData.Framebuffer->Release();
			}

			if (s_VulkanData.RenderPass != nullptr)
			{
				s_VulkanData.RenderPass->Release();
			}

			if (s_VulkanData.GraphicsCommandPool != nullptr)
			{
				s_VulkanData.GraphicsCommandPool->Release();
			}

			if (s_VulkanData.TransferCommandPool != nullptr)
			{
				s_VulkanData.TransferCommandPool->Release();
			}

			if (s_VulkanData.Swapchain != nullptr)
			{
				s_VulkanData.Swapchain->Release();
			}

			if (s_VulkanData.Device != nullptr)
			{
				s_VulkanData.Device->Release();
			}

			if (s_VulkanData.Surface != nullptr)
			{
				s_VulkanData.Surface->Release();
			}

			if (m_Instance != nullptr)
			{
				vkDestroyInstance(m_Instance, nullptr);
				m_Instance = nullptr;
			}

		}

		RendererData& VulkanContext::GetRendererData()
		{
			return s_VulkanData;
		}

		/////////////////////////////////////////
		// Vulkan instance creation and setup
		//

		void VulkanContext::Create(void* windowHandle)
		{
			LOG_CORE_TRACE("[Renderer] Creating Vulkan renderer");

			if (!CreateInstance()) return;

			uint32_t framesInflight = Renderer::GetRendererSettings().FramesInFlight;

			// Init Vulkan

			s_VulkanData.Surface = CreateRef<Surface>(m_Instance, windowHandle);
			s_VulkanData.Device = CreateRef<Device>(m_Instance, s_VulkanData.Surface->GetSurface());

			s_VulkanData.Swapchain = CreateRef<Swapchain>(s_VulkanData.Device, s_VulkanData.Surface->GetSurface(), s_VulkanData.Device->GetDeviceProperties().SurfaceCapabilities.currentExtent);

			s_VulkanData.GraphicsCommandPool = CreateRef<CommandPool>(s_VulkanData.Device, CommandPoolType::Graphics);
			s_VulkanData.TransferCommandPool = CreateRef<CommandPool>(s_VulkanData.Device, CommandPoolType::Transfer);
			s_VulkanData.RenderPass = CreateRef<RenderPass>(s_VulkanData.Device, s_VulkanData.Device->GetDeviceProperties().SurfaceFormat.format);
			s_VulkanData.Framebuffer = CreateRef<Framebuffer>(s_VulkanData.Device, s_VulkanData.Swapchain, s_VulkanData.RenderPass, s_VulkanData.Device->GetDeviceProperties().SurfaceCapabilities.currentExtent);

			for (uint32_t i = 0; i < framesInflight; i++)
			{
				Ref<CommandBuffer> newGraphicsCommandBuffer = CreateRef<CommandBuffer>(s_VulkanData.Device, s_VulkanData.GraphicsCommandPool);
				s_VulkanData.GraphicsCommandBuffers.emplace_back(newGraphicsCommandBuffer);

				Ref<Fence> newInFlightFence = CreateRef<Fence>(s_VulkanData.Device);
				s_VulkanData.InFlightFences.emplace_back(newInFlightFence);

				Ref<Semaphore> newImageAvailableSemaphore = CreateRef<Semaphore>(s_VulkanData.Device);
				s_VulkanData.ImageAvailableSemaphores.emplace_back(newImageAvailableSemaphore);

				Ref<Semaphore> newRenderFinishedSemaphore = CreateRef<Semaphore>(s_VulkanData.Device);
				s_VulkanData.RenderFinishedSemaphores.emplace_back(newRenderFinishedSemaphore);
			}

			// Create pipeline

			ShaderStore::LoadShader("basic", "assets/shaders/basic.shader");
			ShaderStore::LoadShader("flat", "assets/shaders/flat.shader");

			PipelineSpecification pipelineSpecification;
			pipelineSpecification.Extent = s_VulkanData.Device->GetDeviceProperties().SurfaceCapabilities.currentExtent;
			pipelineSpecification.PipelineShader = ShaderStore::GetShader("basic");
			pipelineSpecification.RenderPass = s_VulkanData.RenderPass;

			s_VulkanData.Pipeline = CreateRef<Pipeline>(s_VulkanData.Device, pipelineSpecification);
		}

		bool VulkanContext::CreateInstance()
		{
			VkApplicationInfo applicationInfo = {};
			applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			applicationInfo.apiVersion = VK_API_VERSION_1_3;
			applicationInfo.pEngineName = "Wingnut";
			applicationInfo.engineVersion = 1;
			applicationInfo.pApplicationName = "WingnutApplication";
			applicationInfo.applicationVersion = 1;

			// Find instance layers
			std::vector<std::string> layerProperties = FindInstanceLayers();
			std::vector<const char*> layerPtrs;

			for (auto& layer : layerProperties)
			{
				layerPtrs.emplace_back(layer.c_str());
			}

			// Find instance extensions
			std::vector<std::string> extensionProperties = FindInstanceExtensions();
			std::vector<const char*> extensionPtrs;

			for (auto& extension : extensionProperties)
			{
				extensionPtrs.emplace_back(extension.c_str());
			}

			VkInstanceCreateInfo instanceCreateInfo = {};
			instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			instanceCreateInfo.pApplicationInfo = &applicationInfo;

			instanceCreateInfo.ppEnabledExtensionNames = extensionPtrs.data();
			instanceCreateInfo.enabledExtensionCount = (uint32_t)extensionPtrs.size();

			instanceCreateInfo.ppEnabledLayerNames = layerPtrs.data();
			instanceCreateInfo.enabledLayerCount = (uint32_t)layerPtrs.size();

			if (vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance) != VK_SUCCESS)
			{
				LOG_CORE_ERROR("[Renderer] Failed to create Vulkan instance");
				return false;
			}

			return true;
		}

		std::vector<std::string> VulkanContext::FindInstanceLayers()
		{
			std::vector<std::string> wantedLayerProperties =
			{
				"VK_LAYER_KHRONOS_validation",
			};

			uint32_t propertyCount = 0;
			vkEnumerateInstanceLayerProperties(&propertyCount, nullptr);
			std::vector<VkLayerProperties> layerProperties(propertyCount);
			vkEnumerateInstanceLayerProperties(&propertyCount, layerProperties.data());

			std::vector<std::string> foundProperties;

			LOG_CORE_TRACE("[Renderer] Available instance layers");

			for (auto& property : layerProperties)
			{
				bool match = false;

				for (auto& wantedProperty : wantedLayerProperties)
				{
					if (wantedProperty == property.layerName)
					{
						foundProperties.emplace_back(property.layerName);
						match = true;

						LOG_CORE_WARN(" * {}", property.layerName);
					}
				}

				if (!match)
				{
					LOG_CORE_TRACE(" - {}", property.layerName);
				}
			}

			return foundProperties;
		}

		std::vector<std::string> VulkanContext::FindInstanceExtensions()
		{
			std::vector<std::string> wantedExtensionProperties =
			{
				"VK_KHR_surface",
				"VK_KHR_win32_surface",
			};

			uint32_t propertyCount = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, nullptr);
			std::vector<VkExtensionProperties> extensionProperties(propertyCount);
			vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, extensionProperties.data());

			std::vector<std::string> foundProperties;

			LOG_CORE_TRACE("[Renderer] Available instance extensions");

			for (auto& property : extensionProperties)
			{
				bool match = false;

				for (auto& wantedExtension : wantedExtensionProperties)
				{
					if (wantedExtension == property.extensionName)
					{
						foundProperties.emplace_back(property.extensionName);
						match = true;

						LOG_CORE_WARN(" * {}", property.extensionName);
					}
				}

				if (!match)
				{
					LOG_CORE_TRACE(" - {}", property.extensionName);
				}
			}

			return foundProperties;
		}


		void VulkanContext::BeginScene()
		{
			s_VulkanData.InFlightFences[m_CurrentFrame]->Wait(UINT64_MAX);
			s_VulkanData.InFlightFences[m_CurrentFrame]->Reset();

			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = 0;
			beginInfo.pInheritanceInfo = nullptr;

			if (vkBeginCommandBuffer(s_VulkanData.GraphicsCommandBuffers[m_CurrentFrame]->GetCommandBuffer(), &beginInfo) != VK_SUCCESS)
			{
				LOG_CORE_ERROR("[Renderer] Unable to begin command buffer recording");
				return;
			}

			VkRenderPassBeginInfo renderPassBeginInfo = {};
			renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassBeginInfo.renderPass = s_VulkanData.RenderPass->GetRenderPass();
			renderPassBeginInfo.framebuffer = s_VulkanData.Framebuffer->GetNextFramebuffer();
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent = m_CurrentExtent;

			VkClearValue clearColor = { {{ 0.2f, 0.3f, 0.45f }} };
			renderPassBeginInfo.clearValueCount = 1;
			renderPassBeginInfo.pClearValues = &clearColor;

			vkCmdBeginRenderPass(s_VulkanData.GraphicsCommandBuffers[m_CurrentFrame]->GetCommandBuffer(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline(s_VulkanData.GraphicsCommandBuffers[m_CurrentFrame]->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, s_VulkanData.Pipeline->GetPipeline());

			VkViewport viewport = {};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (float)m_CurrentExtent.width;
			viewport.height = (float)m_CurrentExtent.height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			vkCmdSetViewport(s_VulkanData.GraphicsCommandBuffers[m_CurrentFrame]->GetCommandBuffer(), 0, 1, &viewport);

			VkRect2D scissor = {};
			scissor.offset = { 0, 0 };
			scissor.extent = m_CurrentExtent;
			vkCmdSetScissor(s_VulkanData.GraphicsCommandBuffers[m_CurrentFrame]->GetCommandBuffer(), 0, 1, &scissor);
		}

		void VulkanContext::EndScene()
		{
			vkCmdEndRenderPass(s_VulkanData.GraphicsCommandBuffers[m_CurrentFrame]->GetCommandBuffer());

			if (vkEndCommandBuffer(s_VulkanData.GraphicsCommandBuffers[m_CurrentFrame]->GetCommandBuffer()) != VK_SUCCESS)
			{
				LOG_CORE_ERROR("[Renderer] Unable to end command buffer recording");
				return;
			}

		}

		void VulkanContext::Present()
		{
			uint32_t framesInFlight = Renderer::GetRendererSettings().FramesInFlight;

			uint32_t imageIndex = 0;
			vkAcquireNextImageKHR(s_VulkanData.Device->GetDevice(), s_VulkanData.Swapchain->GetSwapchain(), UINT64_MAX, s_VulkanData.ImageAvailableSemaphores[m_CurrentFrame]->GetSemaphore(), VK_NULL_HANDLE, &imageIndex);

			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

			VkSemaphore waitSemaphores[] = { s_VulkanData.ImageAvailableSemaphores[m_CurrentFrame]->GetSemaphore()};
			VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = waitSemaphores;
			submitInfo.pWaitDstStageMask = waitStages;

			VkCommandBuffer commandBuffers[] = { s_VulkanData.GraphicsCommandBuffers[m_CurrentFrame]->GetCommandBuffer()};

			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = commandBuffers;

			VkSemaphore signalSemaphores[] = { s_VulkanData.RenderFinishedSemaphores[m_CurrentFrame]->GetSemaphore()};

			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = signalSemaphores;

			if (vkQueueSubmit(s_VulkanData.Device->GetQueue(QueueType::Graphics), 1, &submitInfo, s_VulkanData.InFlightFences[m_CurrentFrame]->GetFence()))
			{
				LOG_CORE_ERROR("[Renderer] Unable to submit queue");
			}

			VkPresentInfoKHR presentInfo = {};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = signalSemaphores;

			VkSwapchainKHR swapchains[] = { s_VulkanData.Swapchain->GetSwapchain() };
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = swapchains;
			presentInfo.pImageIndices = &imageIndex;


			if (vkQueuePresentKHR(s_VulkanData.Device->GetQueue(QueueType::Graphics), &presentInfo) != VK_SUCCESS)
			{
				//			LOG_CORE_ERROR("[Renderer] Failed to present queue");
				return;
			}

			m_CurrentFrame = (m_CurrentFrame++) & framesInFlight;
		}

	}

}
