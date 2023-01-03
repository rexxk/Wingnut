#pragma once


#include <vulkan/vulkan.h>



namespace Wingnut
{

	namespace Vulkan
	{


		class DescriptorSet
		{
		public:
			DescriptorSet(VkDescriptorSet descriptorSet, uint32_t setID);
			~DescriptorSet();

		private:
			VkDescriptorSet m_DescriptorSet = nullptr;

			uint32_t m_SetID;


		};


	}

}
