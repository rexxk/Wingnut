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

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 100.0f);

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

		ImGui::Text("Texture");

		ImGui::NextColumn();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

//		if (ImGui::ImageButton((ImTextureID)textureSet, ImVec2(64.0f, 64.0f)))
//		{
//			LOG_CORE_TRACE("Image clicked, should open file dialog and load a new texture");
//		}

		{
			ImGui::Image((ImTextureID)m_SelectedMaterial->GetDescriptor()->GetDescriptor(), ImVec2(64.0f, 64.0f));

			if (ImGui::BeginDragDropTarget())
			{
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TexturePayload");

				if (payload != nullptr)
				{
					uint64_t id = *(uint64_t*)payload->Data;
				}

				ImGui::EndDragDropTarget();
			}


		}


		ImGui::PopStyleVar();

		ImGui::Columns(1);


		ImGui::End();
	}

}
