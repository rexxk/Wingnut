#include "wingnut_pch.h"
#include "MaterialEditorPanel.h"

#include "Assets/ResourceManager.h"

#include "Event/EventUtils.h"
#include "Event/UIEvents.h"

#include "ImGui/Controls/UIImageButton.h"

#include "Renderer/Material.h"

#include "Utils/FileDialog.h"

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

		switch (m_SelectedMaterial->GetType())
		{
			case MaterialType::StaticPBR: DrawPBRMaterial(); break;
			case MaterialType::UI: DrawUIMaterial(); break;
		}

		ImGui::End();
	}

	void MaterialEditorPanel::DrawPBRMaterial()
	{

		PBRMaterialData* materialData = (PBRMaterialData*)m_SelectedMaterial->GetMaterialData();

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

		std::string previewString = ResourceManager::SamplerTypeToString(m_SelectedMaterial->GetSamplerType());

		if (ImGui::BeginCombo("##SamplerTypeCombo", previewString.c_str()))
		{
			for (auto& sampler : ResourceManager::GetSamplerMap())
			{
				bool isSelected = m_SelectedMaterial->GetSamplerType() == sampler.first;

				if (ImGui::Selectable(ResourceManager::SamplerTypeToString(sampler.first).c_str(), &isSelected))
				{
//					m_SelectedMaterial->SetSamplerType(sampler.first);
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

		if (ImGui::DragFloat("##metallness", &materialData->Properties.Metallic, 0.01f, 0.0f, 1.0f))
		{
			m_SelectedMaterial->Update();
		}

		ImGui::NextColumn();

		ImGui::Text("Roughness");

		ImGui::NextColumn();

		if (ImGui::DragFloat("##roughness", &materialData->Properties.Roughness, 0.01f, 0.0f, 1.0f))
		{
			m_SelectedMaterial->Update();
		}

		ImGui::NextColumn();

		ImGui::Text("Albedo Color");

		ImGui::NextColumn();

		//		glm::vec4& albedoColor = ;
		//		ImGui::ColorButton("##colorButton", glm::value_ptr(albedoColor));
		if (ImGui::ColorEdit4("##albedoColor", glm::value_ptr(materialData->Properties.AlbedoColor)))
		{
			m_SelectedMaterial->Update();
		}

		ImGui::NextColumn();

		ImGui::Text("Use Albedo texture");

		ImGui::NextColumn();

		if (ImGui::Checkbox("##useAlbedoTexture", (bool*)&materialData->Properties.UseAlbedoTexture))
		{
			m_SelectedMaterial->Update();
		}

		ImGui::NextColumn();

		ImGui::Text("Albedo Texture");

		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

		{

			UIImageButton albedoImageButton(MaterialTextureType::AlbedoTexture, m_SelectedMaterial);


			if (ImGui::BeginDragDropTarget())
			{
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TexturePayload");

				if (payload != nullptr)
				{
					UUID textureID = *(UUID*)payload->Data;

					m_SelectedMaterial->SetTexture(MaterialTextureType::AlbedoTexture, ResourceManager::GetTexture(textureID));
				}

				ImGui::EndDragDropTarget();
			}

		}

		ImGui::PopStyleVar();

		ImGui::NextColumn();

		ImGui::Text("Use Normal map");

		ImGui::NextColumn();

		if (ImGui::Checkbox("##useNormalMap", (bool*)&materialData->Properties.UseNormalMap))
		{
			m_SelectedMaterial->Update();
		}

		ImGui::NextColumn();

		ImGui::Text("Normal Map");

		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

		{
			UIImageButton(MaterialTextureType::NormalMap, m_SelectedMaterial);

			if (ImGui::BeginDragDropTarget())
			{
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TexturePayload");

				if (payload != nullptr)
				{
					UUID textureID = *(UUID*)payload->Data;

					m_SelectedMaterial->SetTexture(MaterialTextureType::NormalMap, ResourceManager::GetTexture(textureID));
				}

				ImGui::EndDragDropTarget();
			}

		}


		ImGui::PopStyleVar();

		ImGui::NextColumn();

		ImGui::Text("Use Metalness map");

		ImGui::NextColumn();

		if (ImGui::Checkbox("##useMetalnessMap", (bool*)&materialData->Properties.UseMetalnessMap))
		{
			m_SelectedMaterial->Update();
		}

		ImGui::NextColumn();

		ImGui::Text("Metalness Map");

		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

		{
			UIImageButton(MaterialTextureType::MetalnessMap, m_SelectedMaterial);

			if (ImGui::BeginDragDropTarget())
			{
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TexturePayload");

				if (payload != nullptr)
				{
					UUID textureID = *(UUID*)payload->Data;

					m_SelectedMaterial->SetTexture(MaterialTextureType::MetalnessMap, ResourceManager::GetTexture(textureID));
				}

				ImGui::EndDragDropTarget();
			}

		}

		ImGui::PopStyleVar();

		ImGui::NextColumn();

		ImGui::Text("Use Roughness map");

		ImGui::NextColumn();

		if (ImGui::Checkbox("##useRoughnessMap", (bool*)&materialData->Properties.UseRoughnessMap))
		{
			m_SelectedMaterial->Update();
		}

		ImGui::NextColumn();

		ImGui::Text("Roughness Map");

		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

		{
			UIImageButton(MaterialTextureType::RoughnessMap, m_SelectedMaterial);

			if (ImGui::BeginDragDropTarget())
			{
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TexturePayload");

				if (payload != nullptr)
				{
					UUID textureID = *(UUID*)payload->Data;

					m_SelectedMaterial->SetTexture(MaterialTextureType::RoughnessMap, ResourceManager::GetTexture(textureID));
				}

				ImGui::EndDragDropTarget();
			}

		}

		ImGui::PopStyleVar();

		ImGui::NextColumn();

		ImGui::Text("Use Ambient Occlusion map");

		ImGui::NextColumn();

		if (ImGui::Checkbox("##useAmbientOcclusionMap", (bool*)&materialData->Properties.UseAmbientOcclusionMap))
		{
			m_SelectedMaterial->Update();
		}

		ImGui::NextColumn();

		ImGui::Text("Ambient Occlusion Map");

		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

		{
			UIImageButton(MaterialTextureType::AmbientOcclusionMap, m_SelectedMaterial);

			if (ImGui::BeginDragDropTarget())
			{
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TexturePayload");

				if (payload != nullptr)
				{
					UUID textureID = *(UUID*)payload->Data;

					m_SelectedMaterial->SetTexture(MaterialTextureType::AmbientOcclusionMap, ResourceManager::GetTexture(textureID));
				}

				ImGui::EndDragDropTarget();
			}

		}

		ImGui::PopStyleVar();

		ImGui::Columns(1);


	}

	void MaterialEditorPanel::DrawUIMaterial()
	{

	}

}
