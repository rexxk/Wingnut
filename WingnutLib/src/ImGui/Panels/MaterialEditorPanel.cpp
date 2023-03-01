#include "wingnut_pch.h"
#include "MaterialEditorPanel.h"

#include "Assets/MaterialStore.h"
#include "Assets/TextureStore.h"

#include "Event/EventUtils.h"
#include "Event/UIEvents.h"

#include "Renderer/Material.h"

#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>


namespace Wingnut
{


	MaterialEditorPanel::MaterialEditorPanel()
	{
		SubscribeToEvent<MaterialSelectedEvent>([&](MaterialSelectedEvent& event)
			{
				m_SelectedMaterial = event.MaterialID();

				return false;
			});
	}

	MaterialEditorPanel::~MaterialEditorPanel()
	{

	}

	void MaterialEditorPanel::Draw()
	{
		ImGui::Begin("Material Editor");

		if (m_SelectedMaterial == nullptr)
		{
			ImGui::Text("No material selected");
			ImGui::End();

			return;
		}

		auto& properties = m_SelectedMaterial->GetMaterialData().Properties;

		ImGui::Text("Material editor panel");

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 180.0f);

		ImGui::Text("ID");
		ImGui::NextColumn();

		ImGui::Text("%llu", (uint64_t)m_SelectedMaterial->GetID());
		ImGui::NextColumn();

		ImGui::Text("Name");
		ImGui::NextColumn();

		char name[256];
		memset(name, 0, 256);
		std::strcpy(name, m_SelectedMaterial->GetName().c_str());

		if (ImGui::InputText("##name", name, 256))
		{
			m_SelectedMaterial->SetName(std::string(name));
		}

		ImGui::NextColumn();

		ImGui::Text("Sampler");

		ImGui::NextColumn();

		std::string previewString = SamplerStore::SamplerTypeToString(m_SelectedMaterial->GetSamplerType());

		if (ImGui::BeginCombo("##SamplerTypeCombo", previewString.c_str()))
		{
			for (auto& sampler : SamplerStore::GetSamplerMap())
			{
				bool isSelected = m_SelectedMaterial->GetSamplerType() == sampler.first;

				if (ImGui::Selectable(SamplerStore::SamplerTypeToString(sampler.first).c_str(), &isSelected))
				{
					m_SelectedMaterial->SetSamplerType(sampler.first);
				}

				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		ImGui::NextColumn();

		ImGui::Text("Metallness");

		ImGui::NextColumn();

		if (ImGui::DragFloat("##metallness", &properties.Metallic, 0.01f, 0.0f, 1.0f))
		{
			m_SelectedMaterial->Update();
		}

		ImGui::NextColumn();

		ImGui::Text("Roughness");

		ImGui::NextColumn();

		if (ImGui::DragFloat("##roughness", &properties.Roughness, 0.01f, 0.0f, 1.0f))
		{
			m_SelectedMaterial->Update();
		}

		ImGui::NextColumn();

		ImGui::Text("Albedo Color");

		ImGui::NextColumn();

//		glm::vec4& albedoColor = ;
//		ImGui::ColorButton("##colorButton", glm::value_ptr(albedoColor));
		if (ImGui::ColorEdit4("##albedoColor", glm::value_ptr(m_SelectedMaterial->GetMaterialData().Properties.AlbedoColor)))
		{
			m_SelectedMaterial->Update();
		}

		ImGui::NextColumn();
		
		ImGui::Text("Use Albedo texture");

		ImGui::NextColumn();

		if (ImGui::Checkbox("##useAlbedoTexture", (bool*)&properties.UseAlbedoTexture))
		{
			m_SelectedMaterial->Update();
		}

		ImGui::NextColumn();

		ImGui::Text("Albedo Texture");

		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

		{

			ImGui::PushID("##AlbedoImage");

			ImGui::Image((ImTextureID)TextureStore::GetDescriptor(m_SelectedMaterial->GetMaterialData().AlbedoTexture.Texture->GetTextureID())->GetDescriptor(), ImVec2(64.0f, 64.0f));

			if (ImGui::BeginDragDropTarget())
			{
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TexturePayload");

				if (payload != nullptr)
				{
					UUID textureID = *(UUID*)payload->Data;

					m_SelectedMaterial->SetTexture(MaterialType::AlbedoTexture, TextureStore::GetTexture(textureID));
				}

				ImGui::EndDragDropTarget();
			}

			ImGui::PopID();

		}

		ImGui::PopStyleVar();

		ImGui::NextColumn();

		ImGui::Text("Use Normal map");

		ImGui::NextColumn();

		if (ImGui::Checkbox("##useNormalMap", (bool*)&properties.UseNormalMap))
		{
			m_SelectedMaterial->Update();
		}

		ImGui::NextColumn();

		ImGui::Text("Normal Map");

		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

		{
			ImGui::PushID("##NormalMapImage");

			ImGui::Image((ImTextureID)TextureStore::GetDescriptor(m_SelectedMaterial->GetMaterialData().NormalMap.Texture->GetTextureID())->GetDescriptor(), ImVec2(64.0f, 64.0f));

			if (ImGui::BeginDragDropTarget())
			{
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TexturePayload");

				if (payload != nullptr)
				{
					UUID textureID = *(UUID*)payload->Data;

					m_SelectedMaterial->SetTexture(MaterialType::NormalMap, TextureStore::GetTexture(textureID));
				}

				ImGui::EndDragDropTarget();
			}

			ImGui::PopID();
		}


		ImGui::PopStyleVar();

		ImGui::Columns(1);


		ImGui::End();
	}

}
