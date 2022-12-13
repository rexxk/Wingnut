#include "wingnut_pch.h"
#include "Renderer.h"


namespace Wingnut
{

	static RendererData s_VulkanData;



	Renderer::Renderer(void* windowHandle)
	{
		Create(windowHandle);
	}

	Renderer::~Renderer()
	{
		if (s_VulkanData.Pipeline != nullptr)
		{
			s_VulkanData.Pipeline->Release();
		}

		if (s_VulkanData.Framebuffer != nullptr)
		{
			s_VulkanData.Framebuffer->Release();
		}

		if (s_VulkanData.RenderPass != nullptr)
		{
			s_VulkanData.RenderPass->Release();
		}

		if (s_VulkanData.CommandPool != nullptr)
		{
			s_VulkanData.CommandPool->Release();
		}

		if (s_VulkanData.Swapchain != nullptr)
		{
			s_VulkanData.Swapchain->Release();
		}

		if (s_VulkanData.Device != nullptr)
		{
			s_VulkanData.Device->Release();
		}

		if (s_VulkanData.Surface != nullptr)
		{
			s_VulkanData.Surface->Release();
		}

		if (m_Instance != nullptr)
		{
			vkDestroyInstance(m_Instance, nullptr);
			m_Instance = nullptr;
		}

	}

	RendererData& Renderer::GetRendererData()
	{
		return s_VulkanData;
	}


	/////////////////////////////////////////
	// Vulkan instance creation and setup
	//

	void Renderer::Create(void* windowHandle)
	{
		LOG_CORE_TRACE("[Renderer] Creating Vulkan renderer");

		if (!CreateInstance()) return;

		s_VulkanData.Surface = CreateRef<Surface>(m_Instance, windowHandle);
		s_VulkanData.Device = CreateRef<Device>(m_Instance, s_VulkanData.Surface->GetSurface());

		s_VulkanData.Swapchain = CreateRef<Swapchain>(s_VulkanData.Device, s_VulkanData.Surface->GetSurface());
		
		s_VulkanData.CommandPool = CreateRef<CommandPool>(s_VulkanData.Device);
		s_VulkanData.RenderPass = CreateRef<RenderPass>(s_VulkanData.Device, s_VulkanData.Device->GetDeviceProperties().SurfaceFormat.format);
		s_VulkanData.Framebuffer = CreateRef<Framebuffer>(s_VulkanData.Device, s_VulkanData.Swapchain, s_VulkanData.RenderPass, s_VulkanData.Device->GetDeviceProperties().SurfaceCapabilities.currentExtent);

		s_VulkanData.Pipeline = CreateRef<Pipeline>(s_VulkanData.Device, s_VulkanData.RenderPass);
	}

	bool Renderer::CreateInstance()
	{
		VkApplicationInfo applicationInfo = {};
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		applicationInfo.apiVersion = VK_API_VERSION_1_3;
		applicationInfo.pEngineName = "Wingnut";
		applicationInfo.engineVersion = 1;
		applicationInfo.pApplicationName = "WingnutApplication";
		applicationInfo.applicationVersion = 1;

		// Find instance layers
		std::vector<std::string> layerProperties = FindInstanceLayers();
		std::vector<const char*> layerPtrs;

		for (auto& layer : layerProperties)
		{
			layerPtrs.emplace_back(layer.c_str());
		}

		// Find instance extensions
		std::vector<std::string> extensionProperties = FindInstanceExtensions();
		std::vector<const char*> extensionPtrs;

		for (auto& extension : extensionProperties)
		{
			extensionPtrs.emplace_back(extension.c_str());
		}

		VkInstanceCreateInfo instanceCreateInfo = {};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pApplicationInfo = &applicationInfo;

		instanceCreateInfo.ppEnabledExtensionNames = extensionPtrs.data();
		instanceCreateInfo.enabledExtensionCount = (uint32_t)extensionPtrs.size();

		instanceCreateInfo.ppEnabledLayerNames = layerPtrs.data();
		instanceCreateInfo.enabledLayerCount = (uint32_t)layerPtrs.size();

		if (vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance) != VK_SUCCESS)
		{
			LOG_CORE_ERROR("[Renderer] Failed to create Vulkan instance");
			return false;
		}

		return true;
	}

	std::vector<std::string> Renderer::FindInstanceLayers()
	{
		std::vector<std::string> wantedLayerProperties =
		{
			"VK_LAYER_KHRONOS_validation",
		};

		uint32_t propertyCount = 0;
		vkEnumerateInstanceLayerProperties(&propertyCount, nullptr);
		std::vector<VkLayerProperties> layerProperties(propertyCount);
		vkEnumerateInstanceLayerProperties(&propertyCount, layerProperties.data());

		std::vector<std::string> foundProperties;

		LOG_CORE_TRACE("[Renderer] Available instance layers");

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

	std::vector<std::string> Renderer::FindInstanceExtensions()
	{
		std::vector<std::string> wantedExtensionProperties = 
		{
			"VK_KHR_surface",
			"VK_KHR_win32_surface",
		};

		uint32_t propertyCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, nullptr);
		std::vector<VkExtensionProperties> extensionProperties(propertyCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, extensionProperties.data());

		std::vector<std::string> foundProperties;

		LOG_CORE_TRACE("[Renderer] Available instance extensions");

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
