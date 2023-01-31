#include "wingnut_pch.h"
#include "DescriptorPool.h"

#include "Renderer/Renderer.h"


namespace Wingnut
{

	namespace Vulkan
	{

		DescriptorPool::DescriptorPool(Ref<Device> device, uint32_t maxSets)
			: m_Device(device)
		{
			VkDescriptorPoolSize poolSize = {};
			poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			poolSize.descriptorCount = 100; // Renderer::GetRendererSettings().FramesInFlight;

			VkDescriptorPoolCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			createInfo.poolSizeCount = 1;
			createInfo.pPoolSizes = &poolSize;

			createInfo.maxSets = maxSets * Renderer::GetRendererSettings().FramesInFlight;

			if (vkCreateDescriptorPool(m_Device->GetDevice(), &createInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
			{
				LOG_CORE_ERROR("[DescriptorPool] Unable to create pool");
				return;
			}
		}

		DescriptorPool::~DescriptorPool()
		{
			Release();
		}

		void DescriptorPool::Release()
		{
			if (m_DescriptorPool != nullptr)
			{
				vkDestroyDescriptorPool(m_Device->GetDevice(), m_DescriptorPool, nullptr);
				m_DescriptorPool = nullptr;
			}
		}
	}

}