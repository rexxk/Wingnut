#include "wingnut_pch.h"
#include "SceneHierarchy.h"

#include "Scene/Components.h"

#include <imgui.h>



namespace Wingnut
{

	SceneHierarchy::SceneHierarchy(Ref<Scene> scene)
		: m_Scene(scene)
	{

	}

	SceneHierarchy::~SceneHierarchy()
	{

	}

	void SceneHierarchy::Draw()
	{
		ImGui::Begin("Scene Hierarchy");

		for (Entity& entity : m_Scene->GetEntities())
		{
			DrawEntity(entity);
		}
		

		ImGui::End();
	}

	void SceneHierarchy::DrawEntity(Entity& entity)
	{
		TagComponent& tagComponent = entity.GetComponent<TagComponent>();

		if (ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity.ID(), ImGuiTreeNodeFlags_OpenOnArrow, "%s", tagComponent.Tag.c_str()))
		{
			ImGui::Text("%llu", (uint64_t)entity.ID());
				
			ImGui::TreePop();
		}

	}

}
