#include "wingnut_pch.h"
#include "RendererContext.h"

#include "ShaderCompiler.h"

#include "Assets/ShaderStore.h"

#include "Event/EventUtils.h"
#include "Event/WindowEvents.h"


namespace Wingnut
{

	namespace Vulkan
	{

		static RendererData s_VulkanData;


		VulkanContext::VulkanContext(void* windowHandle)
		{
			Create(windowHandle);

			m_CurrentExtent = s_VulkanData.Device->GetDeviceProperties().SurfaceCapabilities.currentExtent;

		}

		VulkanContext::~VulkanContext()
		{

		}

		void VulkanContext::ReleaseAll()
		{
			if (s_VulkanData.Device->GetDevice() != nullptr)
			{
				s_VulkanData.Device->WaitForIdle();
			}


			if (s_VulkanData.DescriptorPool != nullptr)
			{
				s_VulkanData.DescriptorPool->Release();
			}

			ShaderStore::Release();

			if (s_VulkanData.TransferCommandPool != nullptr)
			{
				s_VulkanData.TransferCommandPool->Release();
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

		RendererData& VulkanContext::GetRendererData()
		{
			return s_VulkanData;
		}

		/////////////////////////////////////////
		// Vulkan instance creation and setup
		//

		void VulkanContext::Create(void* windowHandle)
		{
			LOG_CORE_TRACE("[Renderer] Creating Vulkan renderer");

			if (!CreateInstance()) return;

			// Init Vulkan

			s_VulkanData.Surface = CreateRef<Surface>(m_Instance, windowHandle);
			s_VulkanData.Device = CreateRef<Device>(m_Instance, s_VulkanData.Surface->GetSurface());

			s_VulkanData.Swapchain = CreateRef<Swapchain>(s_VulkanData.Device, s_VulkanData.Surface->GetSurface(), s_VulkanData.Device->GetDeviceProperties().SurfaceCapabilities.currentExtent);

			s_VulkanData.TransferCommandPool = CreateRef<CommandPool>(s_VulkanData.Device, CommandPoolType::Transfer);

			// TODO: Max sets hardcoded to 1000
			s_VulkanData.DescriptorPool = CreateRef<DescriptorPool>(s_VulkanData.Device, 1000);

			// Create pipeline

//			ShaderStore::LoadShader("basic", "assets/shaders/basic.shader");
//			ShaderStore::LoadShader("flat", "assets/shaders/flat.shader");

//			PipelineSpecification pipelineSpecification;
//			pipelineSpecification.Extent = s_VulkanData.Device->GetDeviceProperties().SurfaceCapabilities.currentExtent;
//			pipelineSpecification.PipelineShader = ShaderStore::GetShader("basic");
//			pipelineSpecification.RenderPass = s_VulkanData.RenderPass;

//			s_VulkanData.Pipeline = CreateRef<Pipeline>(s_VulkanData.Device, pipelineSpecification);
		}

		bool VulkanContext::CreateInstance()
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

		std::vector<std::string> VulkanContext::FindInstanceLayers()
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

		std::vector<std::string> VulkanContext::FindInstanceExtensions()
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

}
