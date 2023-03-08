#pragma once

#include "Scene/Scene.h"


namespace Wingnut
{

	class MaterialStorePanel
	{
	public:
		MaterialStorePanel(Ref<Scene> activeScene);
		~MaterialStorePanel();

		void Draw();

		void UpdateMaterialList();

	private:

		int m_CurrentSelection = 0;
		int m_ActiveSelection = -1;
		std::vector<const char*> m_ListboxItems;

		Ref<Scene> m_ActiveScene = nullptr;

	};

}
