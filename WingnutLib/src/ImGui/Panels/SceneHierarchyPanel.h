#pragma once

#include "Scene/Scene.h"


namespace Wingnut
{

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel(Ref<Scene> scene);
		~SceneHierarchyPanel();

		void Draw();

	private:
		void DrawEntity(Entity& entity);

	private:
		Ref<Scene> m_Scene = nullptr;

		Entity m_SelectedEntity;
	};


}
