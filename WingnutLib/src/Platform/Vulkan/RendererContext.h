#pragma once

#include "CommandBuffer.h"
#include "CommandPool.h"
#include "DescriptorPool.h"
#include "Device.h"
#include "Fence.h"
#include "Framebuffer.h"
#include "Image.h"
#include "Semaphore.h"
#include "Shader.h"
#include "Surface.h"
#include "Swapchain.h"


#include <vulkan/vulkan.h>


namespace Wingnut
{

	namespace Vulkan
	{

		struct RendererData
		{
			Ref<Device> Device = nullptr;

			Ref<Vulkan::CommandPool> GraphicsCommandPool = nullptr;
			std::vector<Ref<Vulkan::CommandBuffer>> GraphicsCommandBuffers;

			Ref<CommandPool> TransferCommandPool = nullptr;
			Ref<DescriptorPool> DescriptorPool = nullptr;

			Ref<Vulkan::Framebuffer> SceneFramebuffer = nullptr;
			Ref<Vulkan::Framebuffer> UIFramebuffer = nullptr;

			Ref<Vulkan::RenderPass> SceneRenderPass = nullptr;
			Ref<Vulkan::RenderPass> UIRenderPass = nullptr;

			Ref<Swapchain> Swapchain = nullptr;

			Ref<Surface> Surface = nullptr;

			Ref<Image> DepthStencilImage = nullptr;
			
//			Ref<Image> SceneTexture = nullptr;
			Ref<Texture2D> SceneImage = nullptr;

			std::vector<Ref<Vulkan::Semaphore>> ImageAvailableSemaphores;
			std::vector<Ref<Vulkan::Semaphore>> RenderFinishedSemaphores;
			std::vector<Ref<Vulkan::Fence>> InFlightFences;

			// UI globals
			Ref<Vulkan::ImageSampler> DefaultUISampler = nullptr;

			Ref<Vulkan::Texture2D> DefaultUITexture = nullptr;
			Ref<Vulkan::Descriptor> DefaultUITextureDescriptor = nullptr;

		};

		class VulkanContext
		{
		public:
			static Ref<VulkanContext> Create(void* windowHandle = nullptr);

			VulkanContext(void* windowHandle = nullptr);
			~VulkanContext();

			void ReleaseAll();

			uint32_t GetCurrentFrame() const { return m_CurrentFrame; }

			void Present();

			void BeginScene();
			void EndScene();

			void SubmitQueue();

			static RendererData& GetRendererData();

		private:
			void CreateContext(void* windowHandle);

			bool CreateInstance();

			void AcquireImage();

			std::vector<std::string> FindInstanceLayers();
			std::vector<std::string> FindInstanceExtensions();

		private:
			VkInstance m_Instance = nullptr;

			VkExtent2D m_CurrentExtent;

			uint32_t m_CurrentFrame = 0;
			uint32_t m_ImageIndex = 0;
		};

	}

}
