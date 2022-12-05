#pragma once


#include "RendererDevice.h"
#include "RendererSurface.h"


namespace Wingnut
{


	enum class RendererAPI
	{
		None,
		Vulkan,
	};

	struct RendererData
	{
		Ref<RendererDevice> Device;
		Ref<RendererSurface> Surface;

		RendererAPI API;
	};



	class Renderer
	{
	public:
		static Ref<Renderer> Create(RendererAPI api, void* windowHandle);

		virtual RendererData& GetRendererData() = 0;




		static Renderer& Get() { return *s_Instance; }

	private:
		inline static Renderer* s_Instance = nullptr;
	};


}
