#include "wingnut_pch.h"
#include "PropertyPanel.h"

#include "Assets/MaterialStore.h"

#include "Event/EventUtils.h"
#include "Event/UIEvents.h"

#include "Renderer/Material.h"

#include "Scene/Components.h"

#include <glm/gtc/type_ptr.hpp>


#include <imgui_internal.h>


namespace Wingnut
{

	void DrawFloat3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);

		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.75f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));

		if (ImGui::Button("X", buttonSize))
		{
			values.x = resetValue;
		}

		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.75f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));

		if (ImGui::Button("Y", buttonSize))
		{
			values.y = resetValue;
		}

		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.75f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));

		if (ImGui::Button("Z", buttonSize))
		{
			values.z = resetValue;
		}

		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PopStyleVar();
		ImGui::Columns(1);

		ImGui::PopID();
	}



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

			DrawFloat3Control("Translation", transformComponent.Translation);

			glm::vec3 rotationInDegrees = glm::degrees(transformComponent.Rotation);

			DrawFloat3Control("Rotation", rotationInDegrees);

			transformComponent.Rotation = glm::radians(rotationInDegrees);

			DrawFloat3Control("Scale", transformComponent.Scale, 1.0f);

			ImGui::TreePop();

			transformComponent.CalculateTransform();
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
