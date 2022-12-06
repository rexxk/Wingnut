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
		VkSurfaceCapabilitiesKHR SurfaceCapabilities;

		VkSurfaceFormatKHR SurfaceFormat;

		std::vector<VulkanQueueProperty> QueueFamilyProperties;
	};


	class VulkanDevice : public RendererDevice
	{
	public:
		VulkanDevice(VkInstance instance, void* surface);
		virtual ~VulkanDevice();

		virtual void Release() override;

		// TODO: Another lazy assumption
		void* GetDeviceProperties() { return &m_DeviceProperties[0]; }

		VkPhysicalDevice GetPhysicalDevice() { return m_PhysicalDevice; }
		virtual void* GetDevice() override { return m_Device; }

	private:
		void Create(VkInstance instance);
		bool CreatePhysicalDevice(VkInstance instance);
		bool CreateLogicalDevice();

		VulkanPhysicalDeviceProperties GetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice);
		VkSurfaceCapabilitiesKHR GetSurfaceCapabilities(VkPhysicalDevice physicalDevice);
		VkSurfaceFormatKHR GetSurfaceFormat(VkPhysicalDevice physicalDevice);

		std::vector<std::string> FindDeviceLayers();
		std::vector<std::string> FindDeviceExtensions();


	private:
		VkPhysicalDevice m_PhysicalDevice = nullptr;
		VkDevice m_Device = nullptr;
		VkSurfaceKHR m_Surface = nullptr;

		std::vector<VulkanPhysicalDeviceProperties> m_DeviceProperties;
	};

}
