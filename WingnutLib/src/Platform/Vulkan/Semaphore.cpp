#include "wingnut_pch.h"
#include "Semaphore.h"


namespace Wingnut
{

	namespace Vulkan
	{

		Semaphore::Semaphore(Ref<Device> device)
			: m_Device(device)
		{
			VkSemaphoreCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			if (vkCreateSemaphore(m_Device->GetDevice(), &createInfo, nullptr, &m_Semaphore) != VK_SUCCESS)
			{
				LOG_CORE_ERROR("[Renderer] Unable to create semaphore");
				return;
			}
		}

		Semaphore::~Semaphore()
		{
			Release();
		}

		void Semaphore::Release()
		{
			if (m_Semaphore != nullptr)
			{
				vkDestroySemaphore(m_Device->GetDevice(), m_Semaphore, nullptr);
				m_Semaphore = nullptr;
			}
		}

	}
}
