#include "wingnut_pch.h"
#include "SceneHierarchyPanel.h"

#include "Scene/Components.h"

#include "Event/EventUtils.h"
#include "Event/UIEvents.h"

#include <imgui.h>



namespace Wingnut
{

	SceneHierarchyPanel::SceneHierarchyPanel(Ref<Scene> scene)
		: m_Scene(scene), m_SelectedEntity(ECS::EntitySystem::Null)
	{

	}

	SceneHierarchyPanel::~SceneHierarchyPanel()
	{

	}

	void SceneHierarchyPanel::Draw()
	{
		ImGui::Begin("Scene Hierarchy");

		for (Entity& entity : m_Scene->GetEntities())
		{
			DrawEntity(entity);
		}
		

		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntity(Entity& entity)
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

		if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(0))
		{
			m_SelectedEntity = Entity(ECS::EntitySystem::Null);
			AddEventToQueue(CreateRef<EntitySelectedEvent>(m_SelectedEntity));
		}

	}

}
