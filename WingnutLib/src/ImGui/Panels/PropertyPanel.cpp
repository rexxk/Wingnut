#include "wingnut_pch.h"
#include "PropertyPanel.h"

#include "Assets/MaterialStore.h"

#include "Event/EventUtils.h"
#include "Event/UIEvents.h"

#include "Renderer/Material.h"

#include "Scene/Components.h"

#include <glm/gtc/type_ptr.hpp>


namespace Wingnut
{

	PropertyPanel::PropertyPanel()
		: m_SelectedEntity(ECS::EntitySystem::Null)
	{
		SubscribeToEvent<EntitySelectedEvent>([&](EntitySelectedEvent& event)
			{
				m_SelectedEntity = event.GetEntity();

				return false;
			});
	}

	PropertyPanel::~PropertyPanel()
	{

	}

	void PropertyPanel::Draw()
	{

		ImGui::Begin("Properties");

			if (m_SelectedEntity != ECS::EntitySystem::Null)
			{

				DrawTagComponent();

				if (m_SelectedEntity.HasComponent<MeshComponent>())
				{
					DrawMeshComponent();
				}

				if (m_SelectedEntity.HasComponent<TransformComponent>())
				{
					DrawTransformComponent();
				}

				if (m_SelectedEntity.HasComponent<MaterialComponent>())
				{
					DrawMaterialComponent();
				}
			}

		ImGui::End();
	}

	void PropertyPanel::DrawTagComponent()
	{
		if (ImGui::TreeNodeEx("TagComponent", ImGuiTreeNodeFlags_DefaultOpen))
		{
			TagComponent& tagComponent = m_SelectedEntity.GetComponent<TagComponent>();

			char buffer[256];
			memset(buffer, 0, 256);
			std::strcpy(buffer, tagComponent.Tag.c_str());

			if (ImGui::InputText("Tag", buffer, 256))
			{
				tagComponent.Tag = std::string(buffer);
			}

			ImGui::TreePop();
		}

	}

	void PropertyPanel::DrawMeshComponent()
	{
		if (ImGui::TreeNodeEx("MeshComponent", ImGuiTreeNodeFlags_DefaultOpen))
		{
			MeshComponent& meshComponent = m_SelectedEntity.GetComponent<MeshComponent>();

			ImGui::Text("Vertex count: %d", (uint32_t)meshComponent.VertexList.size());
			ImGui::Text("Index count: %d", (uint32_t)meshComponent.IndexList.size());

			ImGui::TreePop();
		}
	}

	void PropertyPanel::DrawTransformComponent()
	{
		if (ImGui::TreeNodeEx("TransformComponent", ImGuiTreeNodeFlags_DefaultOpen))
		{
			TransformComponent& transformComponent = m_SelectedEntity.GetComponent<TransformComponent>();

			ImGui::DragFloat3("Position", glm::value_ptr(transformComponent.Transform[3]), 0.1f);

			ImGui::TreePop();
		}
	}

	void PropertyPanel::DrawMaterialComponent()
	{
		if (ImGui::TreeNodeEx("MaterialComponent", ImGuiTreeNodeFlags_DefaultOpen))
		{
			MaterialComponent& materialComponent = m_SelectedEntity.GetComponent<MaterialComponent>();
			Ref<Material> material = MaterialStore::GetMaterial(materialComponent.MaterialID);

			ImGui::Image((ImTextureID)material->GetDescriptor()->GetDescriptor(), ImVec2(100.0f, 100.0f));

			ImGui::TreePop();
		}
	}

}
