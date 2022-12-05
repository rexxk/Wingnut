#pragma once




namespace Wingnut
{


	class Layer
	{
	public:
		Layer(const std::string& name = "layer");
		virtual ~Layer() {}

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnUpdate() {}

	protected:
		std::string m_Name;
	};


}
