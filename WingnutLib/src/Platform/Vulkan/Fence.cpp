#include "wingnut_pch.h"
#include "Fence.h"



namespace Wingnut
{

	namespace Vulkan
	{

		Ref<Fence> Fence::Create(Ref<Device> device)
		{
			return CreateRef<Fence>(device);
		}


		Fence::Fence(Ref<Device> device)
			: m_Device(device)
		{
			VkFenceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			if (vkCreateFence(m_Device->GetDevice(), &createInfo, nullptr, &m_Fence) != VK_SUCCESS)
			{
				LOG_CORE_ERROR("[Renderer] Unable to create fence");
				return;
			}
		}

		Fence::~Fence()
		{
			Release();
		}

		void Fence::Release()
		{
			if (m_Fence != nullptr)
			{
				vkDestroyFence(m_Device->GetDevice(), m_Fence, nullptr);
				m_Fence = nullptr;
			}
		}

		void Fence::Wait(uint64_t timeout)
		{
			vkWaitForFences(m_Device->GetDevice(), 1, &m_Fence, VK_TRUE, timeout);
		}

		void Fence::Reset()
		{
			vkResetFences(m_Device->GetDevice(), 1, &m_Fence);
		}

	}
}
