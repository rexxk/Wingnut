#pragma once

#include "Timestep.h"



namespace Wingnut
{


	class Layer
	{
	public:
		Layer(const std::string& name = "layer");
		virtual ~Layer() {}

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnUpdate(Timestep ts) {}

		virtual void OnImGuiRender() {}

	protected:
		std::string m_Name;
	};


}
