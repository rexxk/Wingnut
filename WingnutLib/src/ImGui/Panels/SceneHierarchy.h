#pragma once

#include "Scene/Scene.h"


namespace Wingnut
{

	class SceneHierarchy
	{
	public:
		SceneHierarchy(Ref<Scene> scene);
		~SceneHierarchy();

		void Draw();

	private:
		void DrawEntity(UUID uuid);

	private:
		Ref<Scene> m_Scene = nullptr;
	};


}