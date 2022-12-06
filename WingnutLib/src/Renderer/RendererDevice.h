#pragma once




namespace Wingnut
{

	class RendererDevice
	{
	public:
		virtual ~RendererDevice();

		virtual void Release() = 0;
		virtual void* GetDevice() = 0;

		virtual void* GetDeviceProperties() = 0;
	};


}
