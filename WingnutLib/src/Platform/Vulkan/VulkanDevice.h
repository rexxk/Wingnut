#pragma once

#include "Renderer/RendererDevice.h"

#include <vulkan/vulkan.h>


namespace Wingnut
{

	struct VulkanQueueProperty
	{
		uint32_t Index = 0;

		uint32_t Count = 0;
		uint32_t UsedQueues = 0;

		bool Graphics = false;
		bool Compute = false;
		bool Transfer = false;
	};

	struct VulkanPhysicalDeviceProperties
	{
		std::string DeviceName = "";

		VkPhysicalDeviceLimits Limits;

		std::vector<VulkanQueueProperty> QueueFamilyProperties;

	};


	class VulkanDevice : public RendererDevice
	{
	public:
		VulkanDevice(VkInstance instance);
		virtual ~VulkanDevice();

		virtual void Release() override;


		VkPhysicalDevice GetPhysicalDevice() { return m_PhysicalDevice; }
		VkDevice GetDevice() { return m_Device; }

	private:
		void Create(VkInstance instance);
		bool CreatePhysicalDevice(VkInstance instance);
		bool CreateLogicalDevice();

		VulkanPhysicalDeviceProperties GetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice);

		std::vector<std::string> FindDeviceLayers();
		std::vector<std::string> FindDeviceExtensions();

	private:
		VkPhysicalDevice m_PhysicalDevice = nullptr;
		VkDevice m_Device = nullptr;

		std::vector<VulkanPhysicalDeviceProperties> m_DeviceProperties;
	};

}
