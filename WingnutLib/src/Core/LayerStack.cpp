#include "wingnut_pch.h"
#include "LayerStack.h"



namespace Wingnut
{

	LayerStack::LayerStack()
	{

	}

	LayerStack::~LayerStack()
	{
		for (auto layer : m_Layers)
		{
			layer->OnDetach();
		}

		m_Layers.clear();
	}

	void LayerStack::AttachLayer(Ref<Layer> layer)
	{
		m_Layers.emplace_front(layer);

		layer->OnAttach();
	}

	void LayerStack::DetachLayer(Ref<Layer> layer)
	{
		auto location = std::find(m_Layers.begin(), m_Layers.end(), layer);

		if (location != m_Layers.end())
		{
			m_Layers.erase(location);
			layer->OnDetach();
		}
	}

	void LayerStack::AttachOverlay(Ref<Layer> overlay)
	{
		m_Layers.emplace_back(overlay);

		overlay->OnAttach();
	}
	
	void LayerStack::DetachOverlay(Ref<Layer> overlay)
	{
		auto location = std::find(m_Layers.begin(), m_Layers.end(), overlay);

		if (location != m_Layers.end())
		{
			m_Layers.erase(location);
			overlay->OnDetach();
		}
	}

}
