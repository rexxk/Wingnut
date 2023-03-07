#include "wingnut_pch.h"
#include "PropertyPanel.h"

#include "Assets/ResourceManager.h"

#include "Event/EventUtils.h"
#include "Event/UIEvents.h"

#include "Renderer/Material.h"

#include "Scene/Components.h"

#include <glm/gtc/type_ptr.hpp>


#include "ImGui/Controls/UIVector3.h"


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

				if (m_SelectedEntity.HasComponent<LightComponent>())
				{
					DrawLightComponent();
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

	void PropertyPanel::DrawLightComponent()
	{
		if (ImGui::TreeNodeEx("LightComponent", ImGuiTreeNodeFlags_DefaultOpen))
		{
			LightComponent& lightComponent = m_SelectedEntity.GetComponent<LightComponent>();

			Vec3Control positionControl("Position", lightComponent.Position);

			Vec3Control colorControl("Color", lightComponent.Color, 0.0f, 255.0f, 0.1f);

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

			Vec3Control translationControl("Translation", transformComponent.Translation);

//			DrawVec3Control("Translation", transformComponent.Translation);

			glm::vec3 rotationInDegrees = glm::degrees(transformComponent.Rotation);
			Vec3Control rotationControl("Rotation", rotationInDegrees);
//			DrawVec3Control("Rotation", rotationInDegrees);
			transformComponent.Rotation = glm::radians(rotationInDegrees);

			Vec3Control scaleControl("Scale", transformComponent.Scale, 0.0f, 0.0f, 0.1f, 1.0f);
//			DrawVec3Control("Scale", transformComponent.Scale, 1.0f);

			ImGui::TreePop();

			transformComponent.CalculateTransform();
		}
	}

	void PropertyPanel::DrawMaterialComponent()
	{
		bool opened = ImGui::TreeNodeEx("MaterialComponent", ImGuiTreeNodeFlags_DefaultOpen);

		MaterialComponent& materialComponent = m_SelectedEntity.GetComponent<MaterialComponent>();
		Ref<Material> material = ResourceManager::GetMaterial(materialComponent.MaterialID);

		if (opened)
		{
			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, 140.0f);

			ImGui::Text("Material name");
			ImGui::NextColumn();

			//			ImGui::LabelText("", material->GetName().c_str());

			static char inputText[255];
			memset(inputText, 0, 255);
			std::strcpy(inputText, material->GetName().c_str());

			ImGui::LabelText("", "%s", material->GetName().c_str());

//			if (ImGui::InputText("", inputText, 255, ImGuiInputTextFlags_ReadOnly))
//			{
//
//			}

			if (ImGui::BeginDragDropTarget())
			{
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MaterialPayload");

				if (payload != nullptr)
				{
					uint64_t id = *(uint64_t*)payload->Data;

					materialComponent.MaterialID = id;
				}

				ImGui::EndDragDropTarget();
			}

			ImGui::NextColumn();

			ImGui::Text("Preview");

			ImGui::NextColumn();

			ImGui::Image((ImTextureID)ResourceManager::GetDescriptor(material->GetMaterialData().AlbedoTexture.Texture->GetTextureID())->GetDescriptor(), ImVec2(64.0f, 64.0f));

			ImGui::Columns(1);

			ImGui::TreePop();
		}
	}

}
