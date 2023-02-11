#include "wingnut_pch.h"
#include "MaterialStorePanel.h"

#include "Assets/MaterialStore.h"

#include "Event/EventUtils.h"
#include "Event/UIEvents.h"

#include "Renderer/Material.h"

#include <imgui.h>



namespace Wingnut
{


	MaterialStorePanel::MaterialStorePanel()
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
			Ref<MaterialSelectedEvent> event = CreateRef<MaterialSelectedEvent>(MaterialStore::GetMaterialByName(std::string(m_ListboxItems[m_CurrentSelection])));
			AddEventToQueue(event);
			m_ActiveSelection = m_CurrentSelection;
		}

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			UUID materialID = MaterialStore::GetMaterialByName(std::string(m_ListboxItems[m_CurrentSelection]))->GetID();
			ImGui::SetDragDropPayload("MaterialPayload", &materialID, sizeof(uint64_t));

			ImGui::Text("Material");

			ImGui::EndDragDropSource();
		}

//		ImGui::Separator();

		if (ImGui::Button("Create new"))
		{
			MaterialStore::StoreMaterial(Material::Create("newMaterial"));

			UpdateMaterialList();
//			LOG_CORE_TRACE("Adding material");
		}

		ImGui::SameLine();

		if (ImGui::Button("Delete"))
		{

		}

		ImGui::Columns(1);


		ImGui::End();
	}


	void MaterialStorePanel::UpdateMaterialList()
	{
		m_ListboxItems.clear();

		for (auto& materialIterator : MaterialStore::GetMaterialList())
		{
			auto& material = materialIterator.second;
			auto& materialID = materialIterator.first;

			m_ListboxItems.emplace_back(material->GetName().c_str());
		}

	}

}
