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
			static Ref<RenderPass> Create(Ref<Device> device, VkFormat format);

			RenderPass(Ref<Device> device, VkFormat format);
			~RenderPass();

			void Release();

			VkRenderPass GetRenderPass() { return m_RenderPass; }

		private:
			void CreateRenderPass(VkFormat format);

		private:
			VkRenderPass m_RenderPass = nullptr;

			VkDevice m_Device = nullptr;
		};

	}
}
