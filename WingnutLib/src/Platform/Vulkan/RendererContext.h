#pragma once

#include "CommandBuffer.h"
#include "CommandPool.h"
#include "DescriptorPool.h"
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
			Ref<CommandPool> TransferCommandPool = nullptr;
			Ref<Device> Device = nullptr;
			Ref<DescriptorPool> DescriptorPool = nullptr;
//			Ref<Pipeline> Pipeline = nullptr;
			Ref<Surface> Surface = nullptr;

			Ref<Swapchain> Swapchain = nullptr;

		};

		class VulkanContext
		{
		public:
			VulkanContext(void* windowHandle = nullptr);
			~VulkanContext();

			void ReleaseAll();

			uint32_t GetCurrentFrame() const { return m_CurrentFrame; }

			static RendererData& GetRendererData();

		private:
			void Create(void* windowHandle);

			bool CreateInstance();

			std::vector<std::string> FindInstanceLayers();
			std::vector<std::string> FindInstanceExtensions();

		private:
			VkInstance m_Instance = nullptr;

			VkExtent2D m_CurrentExtent;

			uint32_t m_CurrentFrame = 0;
		};

	}

}
