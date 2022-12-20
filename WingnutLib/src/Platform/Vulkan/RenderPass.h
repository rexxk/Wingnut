#pragma once

#include "Device.h"

#include <vulkan/vulkan.h>


namespace Wingnut
{
	namespace Vulkan
	{

		class RenderPass
		{
		public:
			RenderPass(Ref<Device> device, VkFormat format);
			~RenderPass();

			void Release();

			void Create(VkFormat format);

			VkRenderPass GetRenderPass() { return m_RenderPass; }

		private:
			VkRenderPass m_RenderPass = nullptr;

			VkDevice m_Device = nullptr;
		};

	}
}
