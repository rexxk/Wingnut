#include "wingnut_pch.h"
#include "SceneHierarchy.h"

#include "Scene/Components.h"

#include "Event/EventUtils.h"
#include "Event/UIEvents.h"

#include <imgui.h>



namespace Wingnut
{

	SceneHierarchy::SceneHierarchy(Ref<Scene> scene)
		: m_Scene(scene), m_SelectedEntity(ECS::EntitySystem::Null)
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

		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity.ID(), ImGuiTreeNodeFlags_OpenOnArrow | (m_SelectedEntity == entity ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_SpanFullWidth, "%s", tagComponent.Tag.c_str());

		if (ImGui::IsItemClicked())
		{
			if (m_SelectedEntity != entity)
			{
				m_SelectedEntity = entity;
				AddEventToQueue(CreateRef<EntitySelectedEvent>(m_SelectedEntity));
			}

		}

		if (opened)
		{
			ImGui::Text("%llu", (uint64_t)entity.ID());

			ImGui::TreePop();
		}


	}

}
