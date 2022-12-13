#include "wingnut_pch.h"
#include "Device.h"



namespace Wingnut
{


	Device::Device(VkInstance instance, void* surface)
		: m_Surface((VkSurfaceKHR)surface)
	{
		Create(instance);
	}

	Device::~Device()
	{
		Release();
	}

	void Device::Release()
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

	void Device::Create(VkInstance instance)
	{
		LOG_CORE_TRACE("[Renderer] Finding Vulkan device");

		CreatePhysicalDevice(instance);
		CreateLogicalDevice();

		CreateQueues();
	}

	bool Device::CreatePhysicalDevice(VkInstance instance)
	{
		uint32_t physicalDeviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
		std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
		vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

		LOG_CORE_TRACE("[Renderer] Found {} physical device(s)", physicalDeviceCount);


		std::vector<PhysicalDeviceProperties> physicalDeviceProperties;

		// Find best device
		for (auto& physicalDevice : physicalDevices)
		{
			PhysicalDeviceProperties deviceProps = GetPhysicalDeviceProperties(physicalDevice);

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

	bool Device::CreateLogicalDevice()
	{

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		float priorities[] = { 1.0f };

		// TODO: Lazyness here too, first device property selected.
		for (auto& queueFamilyProperty : m_DeviceProperties[0].QueueFamilies[0])
		{
			if (queueFamilyProperty.Graphics == true)
			{
				VkDeviceQueueCreateInfo queueCreateInfo = {};
				queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueCreateInfo.queueFamilyIndex = queueFamilyProperty.FamilyIndex;
				queueCreateInfo.queueCount = 1;

				queueCreateInfo.pQueuePriorities = priorities;

				queueCreateInfos.emplace_back(queueCreateInfo);

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

		if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS)
		{
			LOG_CORE_ERROR("[Renderer] Failed to create Vulkan device");
			return false;
		}

		LOG_CORE_TRACE("[Renderer] Vulkan device created");

		return true;
	}


	//////////////////////////////
	// Physical device functions

	PhysicalDeviceProperties Device::GetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice)
	{
		PhysicalDeviceProperties physicalDeviceProperties;

		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

		if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			LOG_CORE_TRACE("[Renderer] Vulkan device type is not discrete GPU - skipping");
			return PhysicalDeviceProperties();
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

		uint32_t familyIndex = 0;

		for (auto& queueFamilyProperty : queueFamilyProperties)
		{
			QueueFamily queueFamily;

			for (uint32_t queueIndex = 0; queueIndex < queueFamilyProperty.queueCount; queueIndex++)
			{
				if (queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					queueFamily.Graphics = true;
					haveGraphicsQueue = true;
				}

				if (queueFamilyProperty.queueFlags & VK_QUEUE_COMPUTE_BIT)
				{
					queueFamily.Compute = true;
				}

				if (queueFamilyProperty.queueFlags & VK_QUEUE_TRANSFER_BIT)
				{
					queueFamily.Transfer = true;
				}

				queueFamily.FamilyIndex = familyIndex;
				queueFamily.QueueIndex = queueIndex;

				physicalDeviceProperties.QueueFamilies[familyIndex].emplace_back(queueFamily);
			}

			familyIndex++;
		}

		if (!haveGraphicsQueue)
		{
			LOG_CORE_ERROR("[Renderer] Vulkan device does not have a graphics queue");
			return PhysicalDeviceProperties();
		}

		physicalDeviceProperties.SurfaceCapabilities = GetSurfaceCapabilities(physicalDevice);
		physicalDeviceProperties.SurfaceFormat = GetSurfaceFormat(physicalDevice);

		return physicalDeviceProperties;
	}

	VkSurfaceCapabilitiesKHR Device::GetSurfaceCapabilities(VkPhysicalDevice physicalDevice)
	{
		VkSurfaceCapabilitiesKHR surfaceCapabilities;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_Surface, &surfaceCapabilities);

		return surfaceCapabilities;
	}

	VkSurfaceFormatKHR Device::GetSurfaceFormat(VkPhysicalDevice physicalDevice)
	{
		uint32_t surfaceFormatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface, &surfaceFormatCount, nullptr);
		std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface, &surfaceFormatCount, surfaceFormats.data());

		for (auto& format : surfaceFormats)
		{
			if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return format;
			}
		}

		return VkSurfaceFormatKHR();
	}

	//////////////////////////////
	// Logical device functions

	std::vector<std::string> Device::FindDeviceLayers()
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

	std::vector<std::string> Device::FindDeviceExtensions()
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


	//////////////////////////////
	// Queue functions

	QueueFamily& Device::GetQueueFamily(QueueType type)
	{
		for (auto& family : m_DeviceProperties[0].QueueFamilies)
		{
			auto& familyVector = family.second;

			for (auto& familyItem : familyVector)
			{
				if (familyItem.Free != true)
					continue;

				switch (type)
				{
					case QueueType::Graphics:
					{
						if (familyItem.Graphics) 
							return familyItem;

						break;
					}

					case QueueType::Compute:
					{
						if (familyItem.Compute)
							return familyItem;

						break;
					}

					case QueueType::Transfer:
					{
						if (familyItem.Transfer)
							return familyItem;

						break;
					}
				}
			}

		}
	}

	VkQueue Device::GetQueue(QueueType type)
	{
		if (type == QueueType::Graphics)
			return m_GraphicsQueue;

		return nullptr;
	}

	void Device::CreateQueues()
	{
		QueueFamily& familyInfo = GetQueueFamily(QueueType::Graphics);

		vkGetDeviceQueue(m_Device, familyInfo.FamilyIndex, familyInfo.QueueIndex, &m_GraphicsQueue);

		if (m_GraphicsQueue == nullptr)
		{
			LOG_CORE_ERROR("[Renderer] Failed to get graphics queue");
		}

		familyInfo.Free = false;

		LOG_CORE_TRACE("[Renderer] Device queues retrieved")
	}

}
