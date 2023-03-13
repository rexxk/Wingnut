#include "wingnut_pch.h"
#include "Renderer.h"



namespace Wingnut
{



	Ref<Renderer> Renderer::Create(void* windowHandle)
	{
		return CreateRef<Renderer>(windowHandle);
	}



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

	void Renderer::SubmitQueue()
	{
		s_Context->SubmitQueue();
	}

	void Renderer::Present()
	{
		s_Context->Present();
	}

	void Renderer::BeginScene()
	{
		s_Context->BeginScene();
	}

	void Renderer::EndScene()
	{
		s_Context->EndScene();
	}

	void Renderer::WaitForIdle()
	{
		s_Context->GetRendererData().Device->WaitForIdle();
	}


}
