#include "wingnut_pch.h"
#include "Renderer.h"



namespace Wingnut
{




	Renderer::Renderer(void* windowHandle)
	{
		if (s_Instance == nullptr)
		{
			s_Instance = this;
		}

		s_Context = CreateRef<Vulkan::VulkanContext>(windowHandle);
	}

	Renderer::~Renderer()
	{

	}

	void Renderer::Release()
	{
		s_Context->ReleaseAll();
	}

	void Renderer::AcquireImage()
	{
		s_Context->AcquireImage();
	}

	void Renderer::Present()
	{
		s_Context->Present();
	}


}
