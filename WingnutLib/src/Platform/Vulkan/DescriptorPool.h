#pragma once

#include "Device.h"


#include <vulkan/vulkan.h>


namespace Wingnut
{

	namespace Vulkan
	{

		class DescriptorPool
		{
		public:
			DescriptorPool(Ref<Device> device, uint32_t maxSets);
			~DescriptorPool();

			void Release();

			VkDescriptorPool GetDescriptorPool() { return m_DescriptorPool; }

		private:
			VkDescriptorPool m_DescriptorPool = nullptr;

			Ref<Device> m_Device = nullptr;


		};

	}

}
