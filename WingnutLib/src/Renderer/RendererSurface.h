#pragma once




namespace Wingnut
{


	class RendererSurface
	{
	public:
		virtual void Release() = 0;

		virtual void* GetSurface() = 0;
	};


}
