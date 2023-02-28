#include "wingnut_pch.h"
#include "RendererContext.h"

#include "ShaderCompiler.h"

#include "Assets/SamplerStore.h"
#include "Assets/ShaderStore.h"
#include "Assets/TextureStore.h"

#include "Event/EventUtils.h"
#include "Event/WindowEvents.h"


namespace Wingnut
{

	namespace Vulkan
	{

		static RendererData s_VulkanData;


		Ref<VulkanContext> VulkanContext::Create(void* windowHandle)
		{
			return CreateRef<VulkanContext>(windowHandle);
		}


		VulkanContext::VulkanContext(void* windowHandle)
		{
			CreateContext(windowHandle);

			m_CurrentExtent = s_VulkanData.Device->GetDeviceProperties().SurfaceCapabilities.currentExtent;


			SubscribeToEvent<WindowResizedEvent>([&](WindowResizedEvent& event)
				{
					auto& rendererData = Renderer::GetContext()->GetRendererData();

					if (event.Width() == 0 || event.Height() == 0)
						return false;

					VkExtent2D extent = {};
					extent.width = event.Width();
					extent.height = event.Height();

					m_CurrentExtent = extent;

					vkDeviceWaitIdle(rendererData.Device->GetDevice());

					rendererData.Swapchain->Resize((VkSurfaceKHR)rendererData.Surface->GetSurface(), extent);

					rendererData.DepthStencilImage->Release();
					rendererData.DepthStencilImage = CreateRef<Vulkan::Image>(rendererData.Device, Vulkan::ImageType::DepthStencil, (uint32_t)extent.width, (uint32_t)extent.height,
						VK_FORMAT_D32_SFLOAT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_ASPECT_DEPTH_BIT);

					rendererData.SceneImage->Release();
					rendererData.SceneImage = Texture2D::Create(extent.width, extent.height, 4, nullptr, TextureFormat::RenderTarget);

					{
						std::vector<VkImageView> imageViews;
//						imageViews.emplace_back(s_VulkanData.SceneTexture->GetImageView());
						imageViews.emplace_back(s_VulkanData.SceneImage->GetImageView());

						rendererData.SceneFramebuffer->Release();
						rendererData.SceneFramebuffer = Vulkan::Framebuffer::Create(rendererData.Device, rendererData.SceneRenderPass, extent, imageViews, s_VulkanData.DepthStencilImage->GetImageView());
					}

					{
						std::vector<VkImageView> imageViews = s_VulkanData.Swapchain->GetImageViews();

						rendererData.UIFramebuffer->Release();
						rendererData.UIFramebuffer = Vulkan::Framebuffer::Create(rendererData.Device, rendererData.UIRenderPass, extent, imageViews, s_VulkanData.DepthStencilImage->GetImageView());
					}

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

			if (s_VulkanData.DefaultTexture)
			{
				s_VulkanData.DefaultTexture->Release();
			}


			if (s_VulkanData.DescriptorPool != nullptr)
			{
				s_VulkanData.DescriptorPool->Release();
			}

			SamplerStore::Release();
			ShaderStore::Release();

			for (auto& inFlightFence : s_VulkanData.InFlightFences)
			{
				inFlightFence->Release();
			}

			for (auto& imageAvailableSemaphore : s_VulkanData.ImageAvailableSemaphores)
			{
				imageAvailableSemaphore->Release();
			}

			for (auto& renderFinishedSemaphore : s_VulkanData.RenderFinishedSemaphores)
			{
				renderFinishedSemaphore->Release();
			}

			for (auto& graphicsCommandBuffer : s_VulkanData.GraphicsCommandBuffers)
			{
				graphicsCommandBuffer->Release();
			}

			if (s_VulkanData.GraphicsCommandPool != nullptr)
			{
				s_VulkanData.GraphicsCommandPool->Release();
			}


			if (s_VulkanData.TransferCommandPool != nullptr)
			{
				s_VulkanData.TransferCommandPool->Release();
			}

			if (s_VulkanData.UIFramebuffer != nullptr)
			{
				s_VulkanData.UIFramebuffer->Release();
			}

			if (s_VulkanData.SceneFramebuffer != nullptr)
			{
				s_VulkanData.SceneFramebuffer->Release();
			}

			if (s_VulkanData.UIRenderPass != nullptr)
			{
				s_VulkanData.UIRenderPass->Release();
			}

			if (s_VulkanData.SceneRenderPass != nullptr)
			{
				s_VulkanData.SceneRenderPass->Release();
			}

			if (s_VulkanData.SceneImage != nullptr)
			{
				s_VulkanData.SceneImage->Release();
			}

			if (s_VulkanData.DepthStencilImage != nullptr)
			{
				s_VulkanData.DepthStencilImage->Release();
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

		void VulkanContext::CreateContext(void* windowHandle)
		{
			LOG_CORE_TRACE("[Renderer] Creating Vulkan renderer");

			if (!CreateInstance())
			{
				return;
			}

			// Init Vulkan

			s_VulkanData.Surface = Surface::Create(m_Instance, windowHandle);
			s_VulkanData.Device = Device::Create(m_Instance, s_VulkanData.Surface->GetSurface());

			m_CurrentExtent = s_VulkanData.Device->GetDeviceProperties().SurfaceCapabilities.currentExtent;

			RenderPassSpecification renderPassSpecification = {};
			renderPassSpecification.Format = s_VulkanData.Device->GetDeviceProperties().SurfaceFormat.format;
			renderPassSpecification.Target = RenderTarget::Image;
			renderPassSpecification.LoadOp = AttachmentLoadOp::Clear;
			renderPassSpecification.StoreOp = AttachmentStoreOp::Store;

			s_VulkanData.SceneRenderPass = RenderPass::Create(s_VulkanData.Device, renderPassSpecification);

			renderPassSpecification.Target = RenderTarget::Screen;
			renderPassSpecification.LoadOp = AttachmentLoadOp::Clear;
//			renderPassSpecification.LoadOp = AttachmentLoadOp::Load;

			s_VulkanData.UIRenderPass = RenderPass::Create(s_VulkanData.Device, renderPassSpecification);

			s_VulkanData.Swapchain = Swapchain::Create(s_VulkanData.Device, s_VulkanData.Surface->GetSurface(), m_CurrentExtent);

			s_VulkanData.GraphicsCommandPool = CommandPool::Create(s_VulkanData.Device, CommandPoolType::Graphics);
			s_VulkanData.TransferCommandPool = CommandPool::Create(s_VulkanData.Device, CommandPoolType::Transfer);

			// TODO: Max sets hardcoded to 1000
			s_VulkanData.DescriptorPool = DescriptorPool::Create(s_VulkanData.Device, 1000);

			uint32_t framesInflight = Renderer::GetRendererSettings().FramesInFlight;

			for (uint32_t i = 0; i < framesInflight; i++)
			{
				Ref<Vulkan::CommandBuffer> newGraphicsCommandBuffer = CommandBuffer::Create(s_VulkanData.Device, s_VulkanData.GraphicsCommandPool);
				s_VulkanData.GraphicsCommandBuffers.emplace_back(newGraphicsCommandBuffer);

				Ref<Vulkan::Semaphore> newImageAvailableSemaphore = Semaphore::Create(s_VulkanData.Device);
				s_VulkanData.ImageAvailableSemaphores.emplace_back(newImageAvailableSemaphore);

				Ref<Vulkan::Semaphore> newRenderFinishedSemaphore = Semaphore::Create(s_VulkanData.Device);
				s_VulkanData.RenderFinishedSemaphores.emplace_back(newRenderFinishedSemaphore);

				Ref<Vulkan::Fence> newInFlightFence = Fence::Create(s_VulkanData.Device);
				s_VulkanData.InFlightFences.emplace_back(newInFlightFence);
			}

			s_VulkanData.DepthStencilImage = Image::Create(s_VulkanData.Device, Vulkan::ImageType::DepthStencil, m_CurrentExtent.width, m_CurrentExtent.height,
				VK_FORMAT_D32_SFLOAT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_ASPECT_DEPTH_BIT);

			std::vector<VkImageView> imageViews = s_VulkanData.Swapchain->GetImageViews();
			imageViews.emplace_back(s_VulkanData.DepthStencilImage->GetImageView());

			s_VulkanData.SceneImage = Texture2D::Create(m_CurrentExtent.width, m_CurrentExtent.height, 4, nullptr, TextureFormat::RenderTarget);

			{
				std::vector<VkImageView> imageViews;
				imageViews.emplace_back(s_VulkanData.SceneImage->GetImageView());

				s_VulkanData.SceneFramebuffer = Vulkan::Framebuffer::Create(s_VulkanData.Device, s_VulkanData.SceneRenderPass, m_CurrentExtent, imageViews, s_VulkanData.DepthStencilImage->GetImageView());
			}

			{
				std::vector<VkImageView> imageViews = s_VulkanData.Swapchain->GetImageViews();

				s_VulkanData.UIFramebuffer = Vulkan::Framebuffer::Create(s_VulkanData.Device, s_VulkanData.UIRenderPass, m_CurrentExtent, imageViews, s_VulkanData.DepthStencilImage->GetImageView());
			}


			ShaderStore::LoadShader(ShaderType::Default, "assets/shaders/Basic.shader");
			ShaderStore::LoadShader(ShaderType::ImGui, "assets/shaders/ImGui.shader");

			SamplerStore::AddSampler(SamplerType::LinearRepeat, Vulkan::ImageSampler::Create(s_VulkanData.Device, Vulkan::ImageSamplerFilter::Linear, Vulkan::ImageSamplerMode::Repeat));
			SamplerStore::AddSampler(SamplerType::NearestRepeat, Vulkan::ImageSampler::Create(s_VulkanData.Device, Vulkan::ImageSamplerFilter::Nearest, Vulkan::ImageSamplerMode::Repeat));

			s_VulkanData.DefaultTexture = Vulkan::Texture2D::Create("assets/textures/checkerboard.png", Vulkan::TextureFormat(Vulkan::TextureFormat::R8G8B8A8_Normalized));
			s_VulkanData.DefaultTextureDescriptor = Vulkan::Descriptor::Create(s_VulkanData.Device, ShaderStore::GetShader(ShaderType::ImGui), ImGuiTextureDescriptor);
			s_VulkanData.DefaultTextureDescriptor->SetImageBinding(0, s_VulkanData.DefaultTexture, SamplerStore::GetSampler(SamplerType::Default));
			s_VulkanData.DefaultTextureDescriptor->UpdateBindings();

			TextureStore::AddTextureData(s_VulkanData.DefaultTexture, s_VulkanData.DefaultTextureDescriptor);
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


		void VulkanContext::AcquireImage()
		{
			auto& rendererData = Renderer::GetContext()->GetRendererData();

			vkAcquireNextImageKHR(rendererData.Device->GetDevice(), rendererData.Swapchain->GetSwapchain(), UINT64_MAX, s_VulkanData.ImageAvailableSemaphores[m_CurrentFrame]->GetSemaphore(), VK_NULL_HANDLE, &m_ImageIndex);
		}


		void VulkanContext::Present()
		{
			auto& rendererData = Renderer::GetContext()->GetRendererData();
			uint32_t framesInFlight = Renderer::GetRendererSettings().FramesInFlight;

			std::vector<VkSemaphore> signalSemaphores = { s_VulkanData.RenderFinishedSemaphores[m_CurrentFrame]->GetSemaphore() };

			VkPresentInfoKHR presentInfo = {};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.waitSemaphoreCount = (uint32_t)signalSemaphores.size();
			presentInfo.pWaitSemaphores = signalSemaphores.data();

			VkSwapchainKHR swapchains[] = { rendererData.Swapchain->GetSwapchain() };
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = swapchains;
			presentInfo.pImageIndices = &m_ImageIndex;


			if (vkQueuePresentKHR(rendererData.Device->GetQueue(Vulkan::QueueType::Graphics), &presentInfo) != VK_SUCCESS)
			{
				//			LOG_CORE_ERROR("[Renderer] Failed to present queue");
				return;
			}

			m_CurrentFrame = (m_CurrentFrame++) & framesInFlight;

			Renderer::GetContext()->GetRendererData().Device->WaitForIdle();
		}

		void VulkanContext::BeginScene()
		{
			auto& commandBuffer = s_VulkanData.GraphicsCommandBuffers[m_CurrentFrame];

			s_VulkanData.InFlightFences[m_CurrentFrame]->Wait(UINT64_MAX);
			s_VulkanData.InFlightFences[m_CurrentFrame]->Reset();

			AcquireImage();


			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = 0;
			beginInfo.pInheritanceInfo = nullptr;

			if (vkBeginCommandBuffer(s_VulkanData.GraphicsCommandBuffers[m_CurrentFrame]->GetCommandBuffer(), &beginInfo) != VK_SUCCESS)
			{
				LOG_CORE_ERROR("[ImGuiRenderer] Unable to begin command buffer recording");
				return;
			}

			VkRenderPassBeginInfo renderPassBeginInfo = {};
			renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassBeginInfo.renderPass = s_VulkanData.SceneRenderPass->GetRenderPass();
			renderPassBeginInfo.framebuffer = s_VulkanData.SceneFramebuffer->GetNextFramebuffer();
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent = m_CurrentExtent;

			std::array<VkClearValue, 3> clearValues = {};
			clearValues[0].color = { {0.2f, 0.3f, 0.45f, 1.0f} };
			clearValues[1].depthStencil = { 1.0f, 0 };
			clearValues[2].color = { {0.2f, 0.3f, 0.45f, 1.0f} };

			renderPassBeginInfo.clearValueCount = (uint32_t)clearValues.size();
			renderPassBeginInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(commandBuffer->GetCommandBuffer(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		}

		void VulkanContext::EndScene()
		{
			auto& commandBuffer = s_VulkanData.GraphicsCommandBuffers[m_CurrentFrame];

//			vkCmdEndRenderPass(commandBuffer->GetCommandBuffer());

			if (vkEndCommandBuffer(commandBuffer->GetCommandBuffer()) != VK_SUCCESS)
			{
				LOG_CORE_ERROR("[ImGuiRenderer] Unable to end command buffer recording");
				return;
			}

		}

		void VulkanContext::SubmitQueue()
		{
			auto& rendererData = Renderer::GetContext()->GetRendererData();
			auto& commandBuffer = s_VulkanData.GraphicsCommandBuffers[m_CurrentFrame];

			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

			std::vector<VkSemaphore> waitSemaphores = { rendererData.ImageAvailableSemaphores[m_CurrentFrame]->GetSemaphore() };
			std::vector<VkPipelineStageFlags> waitStages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
			submitInfo.waitSemaphoreCount = (uint32_t)waitSemaphores.size();
			submitInfo.pWaitSemaphores = waitSemaphores.data();
			submitInfo.pWaitDstStageMask = waitStages.data();

			std::vector<VkCommandBuffer> commandBuffers = { commandBuffer->GetCommandBuffer() };

			submitInfo.commandBufferCount = (uint32_t)commandBuffers.size();
			submitInfo.pCommandBuffers = commandBuffers.data();

			std::vector<VkSemaphore> signalSemaphores = { rendererData.RenderFinishedSemaphores[m_CurrentFrame]->GetSemaphore() };

			submitInfo.signalSemaphoreCount = (uint32_t)signalSemaphores.size();
			submitInfo.pSignalSemaphores = signalSemaphores.data();

			if (vkQueueSubmit(rendererData.Device->GetQueue(Vulkan::QueueType::Graphics), 1, &submitInfo, rendererData.InFlightFences[m_CurrentFrame]->GetFence()))
			{
				LOG_CORE_ERROR("[ImGuiRenderer] Unable to submit queue");
			}

		}

	}

}
