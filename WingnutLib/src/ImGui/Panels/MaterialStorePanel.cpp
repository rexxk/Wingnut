#include "wingnut_pch.h"
#include "MaterialStorePanel.h"

#include "Assets/ResourceManager.h"

#include "Event/EventUtils.h"
#include "Event/UIEvents.h"

#include "Renderer/Material.h"

#include "Scene/Components.h"

#include <imgui.h>



namespace Wingnut
{


	MaterialStorePanel::MaterialStorePanel(Ref<Scene> activeScene)
		: m_ActiveScene(activeScene)
	{
		UpdateMaterialList();
	}

	MaterialStorePanel::~MaterialStorePanel()
	{

	}

	void MaterialStorePanel::Draw()
	{
		ImGui::Begin("Materials");

			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, 100.0f);

			ImGui::Text("Materials");
			ImGui::NextColumn();

			if (ImGui::ListBox("##materials", &m_CurrentSelection, m_ListboxItems.data(), (uint32_t)m_ListboxItems.size(), 10))
			{
				Ref<MaterialSelectedEvent> event = CreateRef<MaterialSelectedEvent>(ResourceManager::GetMaterialByName(std::string(m_ListboxItems[m_CurrentSelection])));
				AddEventToQueue(event);
				m_ActiveSelection = m_CurrentSelection;
			}

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				UUID materialID = ResourceManager::GetMaterialByName(std::string(m_ListboxItems[m_CurrentSelection]))->GetID();
				ImGui::SetDragDropPayload("MaterialPayload", &materialID, sizeof(uint64_t));

				ImGui::Text("Material");

				ImGui::EndDragDropSource();
			}

//		ImGui::Separator();

			if (ImGui::Button("Create new"))
			{
				ResourceManager::StoreMaterial(Material::Create("newMaterial_" + std::to_string((uint32_t)m_ListboxItems.size()), ResourceManager::GetShader(ShaderType::Default)));

				UpdateMaterialList();
			}

			ImGui::SameLine();

			if (ImGui::Button("Delete"))
			{
				if (std::string(m_ListboxItems[m_CurrentSelection]) != "Default")
				{
					Ref<Material> material = ResourceManager::GetMaterialByName(std::string(m_ListboxItems[m_CurrentSelection]));
					UUID materialID = material->GetID();

					for (auto& entity : m_ActiveScene->GetEntities())
					{
						if (entity.HasComponent<MaterialComponent>())
						{
							MaterialComponent& materialComponent = entity.GetComponent<MaterialComponent>();

							if (materialComponent.MaterialID == materialID)
							{
								materialComponent.MaterialID = ResourceManager::GetMaterialByName("Default")->GetID();
								break;
							}
						}
					}

					for (auto iterator = m_ListboxItems.begin(); iterator != m_ListboxItems.end(); iterator++)
					{
						if (*(iterator) == m_ListboxItems[m_CurrentSelection])
						{
							m_ListboxItems.erase(iterator);
							break;
						}
					}

					m_CurrentSelection = 0;

					Ref<MaterialSelectedEvent> event = CreateRef<MaterialSelectedEvent>(nullptr);
					AddEventToQueue(event);

					ResourceManager::DeleteMaterial(material->GetType(), material->GetName());
				}

			}

			ImGui::Columns(1);

		ImGui::End();
	}


	void MaterialStorePanel::UpdateMaterialList()
	{
		m_ListboxItems.clear();

		for (auto& materialIterator : ResourceManager::GetMaterialList())
		{
			auto& materialVector = materialIterator.second;
//			auto& materialID = materialIterator.first;

			for (auto& material : materialVector)
			{
				m_ListboxItems.emplace_back(material->GetName().c_str());
			}
		}

	}

}
