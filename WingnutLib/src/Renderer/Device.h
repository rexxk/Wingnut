#pragma once

#include <vulkan/vulkan.h>


namespace Wingnut
{

	struct QueueProperty
	{
		uint32_t Index = 0;

		uint32_t Count = 0;
		uint32_t UsedQueues = 0;

		bool Graphics = false;
		bool Compute = false;
		bool Transfer = false;
	};

	struct PhysicalDeviceProperties
	{
		std::string DeviceName = "";

		VkPhysicalDeviceLimits Limits;
		VkSurfaceCapabilitiesKHR SurfaceCapabilities;

		VkSurfaceFormatKHR SurfaceFormat;

		std::vector<QueueProperty> QueueFamilyProperties;
	};


	class Device
	{
	public:
		Device(VkInstance instance, void* surface);
		virtual ~Device();

		void Release();

		// TODO: Another lazy assumption
		PhysicalDeviceProperties GetDeviceProperties() { return m_DeviceProperties[0]; }

		VkPhysicalDevice GetPhysicalDevice() { return m_PhysicalDevice; }
		VkDevice GetDevice() { return m_Device; }

	private:
		void Create(VkInstance instance);
		bool CreatePhysicalDevice(VkInstance instance);
		bool CreateLogicalDevice();

		PhysicalDeviceProperties GetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice);
		VkSurfaceCapabilitiesKHR GetSurfaceCapabilities(VkPhysicalDevice physicalDevice);
		VkSurfaceFormatKHR GetSurfaceFormat(VkPhysicalDevice physicalDevice);

		std::vector<std::string> FindDeviceLayers();
		std::vector<std::string> FindDeviceExtensions();


	private:
		VkPhysicalDevice m_PhysicalDevice = nullptr;
		VkDevice m_Device = nullptr;
		VkSurfaceKHR m_Surface = nullptr;

		std::vector<PhysicalDeviceProperties> m_DeviceProperties;
	};

}
