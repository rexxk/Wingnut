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


	void Renderer::BeginScene(Ref<Vulkan::Pipeline> pipeline)
	{
		s_Context->BeginScene(pipeline);
	}

	void Renderer::EndScene()
	{
		s_Context->EndScene();
	}

	void Renderer::Present()
	{
		s_Context->Present();
	}

}
