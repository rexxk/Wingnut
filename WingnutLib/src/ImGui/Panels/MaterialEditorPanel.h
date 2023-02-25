#pragma once

#include "Assets/SamplerStore.h"


namespace Wingnut
{


	class Material;


	class MaterialEditorPanel
	{
	public:
		MaterialEditorPanel();
		~MaterialEditorPanel();

		void Draw();

	private:
		Ref<Material> m_SelectedMaterial = nullptr;

		int m_CurrentSamplerSelection = 0;
	};



}
