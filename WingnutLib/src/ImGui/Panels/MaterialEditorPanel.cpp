#include "wingnut_pch.h"
#include "MaterialEditorPanel.h"

#include "Assets/MaterialStore.h"

#include "Event/EventUtils.h"
#include "Event/UIEvents.h"

#include "Renderer/Material.h"

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

		ImGui::Text("Material editor panel");

//		ImGui::Text("ID: %llu", (uint64_t)m_SelectedMaterial->ID());

		ImGui::Columns(2);

		ImGui::SetColumnWidth(0, 100.0f);

		ImGui::Text("Texture");

		ImGui::NextColumn();

		ImGui::Text("Texture placeholder");

		if (m_SelectedMaterial->GetDescriptor() != nullptr)
		{
			ImGui::Image((ImTextureID)m_SelectedMaterial->GetDescriptor()->GetDescriptor(), ImVec2(100.0f, 100.0f));
		}

		ImGui::Columns(1);


		ImGui::End();
	}

}
