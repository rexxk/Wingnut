#pragma once




namespace Wingnut
{

	class RendererDevice
	{
	public:
		virtual ~RendererDevice();

		virtual void Release() = 0;
	};


}
