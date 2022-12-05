#include "wingnut_pch.h"
#include "VulkanDevice.h"



namespace Wingnut
{


	VulkanDevice::VulkanDevice(VkInstance instance)
	{
		Create(instance);
	}

	VulkanDevice::~VulkanDevice()
	{
		Release();
	}

	void VulkanDevice::Release()
	{
		if (m_Device)
		{
			vkDestroyDevice(m_Device, nullptr);
			m_Device = nullptr;
		}

		if (m_PhysicalDevice)
		{
			m_PhysicalDevice = nullptr;
		}
	}

	void VulkanDevice::Create(VkInstance instance)
	{
		LOG_CORE_TRACE("[Renderer] Finding Vulkan device");

		CreatePhysicalDevice(instance);
		CreateLogicalDevice();
	}

	bool VulkanDevice::CreatePhysicalDevice(VkInstance instance)
	{
		uint32_t physicalDeviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
		std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
		vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

		LOG_CORE_TRACE("[Renderer] Found {} physical device(s)", physicalDeviceCount);


		std::vector<VulkanPhysicalDeviceProperties> physicalDeviceProperties;

		// Find best device
		for (auto& physicalDevice : physicalDevices)
		{
			VulkanPhysicalDeviceProperties deviceProps = GetPhysicalDeviceProperties(physicalDevice);

			if (deviceProps.DeviceName != "")
			{
				physicalDeviceProperties.emplace_back(deviceProps);

				LOG_CORE_TRACE(" - {}", deviceProps.DeviceName);

				// TODO: Rank this selection, I'm only lazy to choose first discrete GPU.
				if (m_PhysicalDevice == nullptr)
				{
					m_PhysicalDevice = physicalDevice;
					m_DeviceProperties = physicalDeviceProperties;
				}
			}
		}


		return true;
	}

	bool VulkanDevice::CreateLogicalDevice()
	{

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		float priorities[] = { 1.0f };

		uint32_t index = 0;

		// TODO: Lazyness here too, first device property selected.
		for (auto& queueFamilyProperty : m_DeviceProperties[0].QueueFamilyProperties)
		{
			if (queueFamilyProperty.Graphics == true)
			{
				VkDeviceQueueCreateInfo queueCreateInfo = {};
				queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueCreateInfo.queueFamilyIndex = index++;
				queueCreateInfo.queueCount = 1;

				queueCreateInfo.pQueuePriorities = priorities;

				queueCreateInfos.emplace_back(queueCreateInfo);

				queueFamilyProperty.UsedQueues++;

				break;
			}
		}

		std::vector<std::string> deviceLayers = FindDeviceLayers();
		std::vector<const char*> deviceLayersPointers;

		for (auto& deviceLayer : deviceLayers)
		{
			deviceLayersPointers.emplace_back(deviceLayer.c_str());
		}

		std::vector<std::string> deviceExtensions = FindDeviceExtensions();
		std::vector<const char*> deviceExtensionsPointers;

		for (auto& deviceExtension : deviceExtensions)
		{
			deviceExtensionsPointers.emplace_back(deviceExtension.c_str());
		}

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();

		createInfo.ppEnabledLayerNames = deviceLayersPointers.data();
		createInfo.enabledLayerCount = (uint32_t)deviceLayersPointers.size();

		createInfo.ppEnabledExtensionNames = deviceExtensionsPointers.data();
		createInfo.enabledExtensionCount = (uint32_t)deviceExtensionsPointers.size();

		vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device);


		return true;
	}


	//////////////////////////////
	// Physical device functions

	VulkanPhysicalDeviceProperties VulkanDevice::GetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice)
	{
		VulkanPhysicalDeviceProperties physicalDeviceProperties;

		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

		if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			LOG_CORE_TRACE("[Renderer] Vulkan device type is not discrete GPU - skipping");
			return VulkanPhysicalDeviceProperties();
		}

		physicalDeviceProperties.DeviceName = deviceProperties.deviceName;
		physicalDeviceProperties.Limits = deviceProperties.limits;

		// Get queue family properties

		uint32_t queueFamilyPropertyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, queueFamilyProperties.data());

		LOG_CORE_TRACE(" - {} queue families", queueFamilyPropertyCount);

		bool haveGraphicsQueue = false;

		for (auto& queueFamilyProperty : queueFamilyProperties)
		{
			VulkanQueueProperty queueProperties;
			queueProperties.Count = queueFamilyProperty.queueCount;

			if (queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				queueProperties.Graphics = true;
				haveGraphicsQueue = true;
			}

			if (queueFamilyProperty.queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				queueProperties.Compute = true;
			}

			if (queueFamilyProperty.queueFlags & VK_QUEUE_TRANSFER_BIT)
			{
				queueProperties.Transfer = true;
			}

			physicalDeviceProperties.QueueFamilyProperties.emplace_back(queueProperties);
		}

		if (!haveGraphicsQueue)
		{
			LOG_CORE_ERROR("[Renderer] Vulkan device does not have a graphics queue");
			return VulkanPhysicalDeviceProperties();
		}

		return physicalDeviceProperties;
	}


	//////////////////////////////
	// Logical device functions

	std::vector<std::string> VulkanDevice::FindDeviceLayers()
	{
		std::vector<std::string> wantedLayerProperties =
		{
			"VK_LAYER_KHRONOS_validation",
		};

		uint32_t propertyCount = 0;
		vkEnumerateDeviceLayerProperties(m_PhysicalDevice, &propertyCount, nullptr);
		std::vector<VkLayerProperties> layerProperties(propertyCount);
		vkEnumerateDeviceLayerProperties(m_PhysicalDevice, &propertyCount, layerProperties.data());

		std::vector<std::string> foundProperties;

		LOG_CORE_TRACE("[Renderer] Available device layers");

		for (auto& property : layerProperties)
		{
			bool match = false;

			for (auto& wantedProperty : wantedLayerProperties)
			{
				if (wantedProperty == property.layerName)
				{
					foundProperties.emplace_back(property.layerName);
					match = true;

					LOG_CORE_WARN(" * {}", property.layerName);
				}
			}

			if (!match)
			{
				LOG_CORE_TRACE(" - {}", property.layerName);
			}
		}

		return foundProperties;
	}

	std::vector<std::string> VulkanDevice::FindDeviceExtensions()
	{
		std::vector<std::string> wantedExtensionProperties =
		{
			"VK_KHR_swapchain",
		};

		uint32_t propertyCount = 0;
		vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &propertyCount, nullptr);
		std::vector<VkExtensionProperties> extensionProperties(propertyCount);
		vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &propertyCount, extensionProperties.data());

		std::vector<std::string> foundProperties;

		LOG_CORE_TRACE("[Renderer] Available device extensions");

		for (auto& property : extensionProperties)
		{
			bool match = false;

			for (auto& wantedExtension : wantedExtensionProperties)
			{
				if (wantedExtension == property.extensionName)
				{
					foundProperties.emplace_back(property.extensionName);
					match = true;

					LOG_CORE_WARN(" * {}", property.extensionName);
				}
			}

			if (!match)
			{
				LOG_CORE_TRACE(" - {}", property.extensionName);
			}
		}

		return foundProperties;
	}

}
