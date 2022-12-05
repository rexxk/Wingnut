#pragma once

#include "Renderer/RendererDevice.h"



namespace Wingnut
{

	class VulkanDevice : public RendererDevice
	{
	public:
		VulkanDevice();
		virtual ~VulkanDevice();

		virtual void Release() override;
	};

}
