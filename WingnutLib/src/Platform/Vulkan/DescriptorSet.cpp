#include "wingnut_pch.h"
#include "DescriptorSet.h"



namespace Wingnut
{

	namespace Vulkan
	{

		DescriptorSet::DescriptorSet(VkDescriptorSet descriptorSet, uint32_t setID)
			: m_DescriptorSet(descriptorSet), m_SetID(setID)
		{

		}

		DescriptorSet::~DescriptorSet()
		{

		}

	}

}
