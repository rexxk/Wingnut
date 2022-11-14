#pragma once

#include "Layer.h"


namespace Wingnut
{

	class LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void AttachLayer(Ref<Layer> layer);
		void DetachLayer(Ref<Layer> layer);

		void AttachOverlay(Ref<Layer> overlay);
		void DetachOverlay(Ref<Layer> overlay);

		std::list<Ref<Layer>>::iterator begin() { return m_Layers.begin(); }
		std::list<Ref<Layer>>::iterator end() { return m_Layers.end(); }

		const std::list<Ref<Layer>>::const_iterator cbegin() const { return m_Layers.cbegin(); }
		const std::list<Ref<Layer>>::const_iterator cend() const { return m_Layers.cend(); }

	private:
		std::list<Ref<Layer>> m_Layers;

	};

}
