#pragma once

#include "CommandBuffer.h"
#include "CommandPool.h"
#include "Device.h"
#include "Fence.h"
#include "Framebuffer.h"
#include "Pipeline.h"
#include "RenderPass.h"
#include "Semaphore.h"
#include "Surface.h"
#include "Swapchain.h"


#include <vulkan/vulkan.h>


namespace Wingnut
{

	namespace Vulkan
	{

		struct RendererData
		{
			Ref<CommandBuffer> GraphicsCommandBuffer = nullptr;
			Ref<CommandPool> GraphicsCommandPool = nullptr;
			Ref<CommandPool> TransferCommandPool = nullptr;
			Ref<Device> Device = nullptr;
			Ref<Framebuffer> Framebuffer = nullptr;
			Ref<Pipeline> Pipeline = nullptr;
			Ref<RenderPass> RenderPass = nullptr;
			Ref<Surface> Surface = nullptr;
			Ref<Swapchain> Swapchain = nullptr;

			Ref<Fence> InFlightFence = nullptr;
			Ref<Semaphore> ImageAvailableSemaphore = nullptr;
			Ref<Semaphore> RenderFinishedSemaphore = nullptr;
		};

		class VulkanContext
		{
		public:
			VulkanContext(void* windowHandle = nullptr);
			~VulkanContext();

			void ReleaseAll();


			void BeginScene();
			void EndScene();

			void Present();

			static RendererData& GetRendererData();

		private:
			void Create(void* windowHandle);

			bool CreateInstance();

			std::vector<std::string> FindInstanceLayers();
			std::vector<std::string> FindInstanceExtensions();

		private:
			VkInstance m_Instance = nullptr;

			VkExtent2D m_CurrentExtent;

		};

	}

}
