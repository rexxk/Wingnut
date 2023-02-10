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
		for (auto& materialIterator : MaterialStore::GetMaterialList())
		{
			auto& material = materialIterator.second;
			auto& materialID = materialIterator.first;

			m_ListboxItems.emplace_back(material->GetName().c_str());
		}
	}

	MaterialStorePanel::~MaterialStorePanel()
	{

	}

	void MaterialStorePanel::Draw()
	{
		ImGui::Begin("Materials");

		if (ImGui::ListBox("Materials", &m_CurrentSelection, m_ListboxItems.data(), (uint32_t)m_ListboxItems.size(), 10))
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

		ImGui::End();
	}

}
