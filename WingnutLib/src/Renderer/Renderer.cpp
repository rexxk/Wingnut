#include "wingnut_pch.h"
#include "Renderer.h"

#include "Platform/Vulkan/VulkanRenderer.h"


namespace Wingnut
{


	Ref<Renderer> Renderer::Create(RendererAPI api)
	{
		switch (api)
		{
			case RendererAPI::Vulkan: return CreateRef<VulkanRenderer>();
		}

		LOG_CORE_TRACE("[Renderer] No renderer API defined");

		return nullptr;
	}



}
