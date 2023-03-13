#include "wingnut_pch.h"
#include "UIVector3.h"

#include "imgui.h"
#include "imgui_internal.h"


namespace Wingnut
{


	Vec3Control::Vec3Control(const std::string& label, glm::vec3& value, float minValue, float maxValue, float step, float resetValue, float columnWidth)
		: m_Label(label), m_Value(value), m_MinValue(minValue), m_MaxValue(maxValue), m_Step(step), m_ResetValue(resetValue), m_ColumnWidth(columnWidth)
	{
		Draw();
	}

	void Vec3Control::Draw()
	{
		ImGui::PushID(m_Label.c_str());

		ImGui::Columns(2);

		ImGui::SetColumnWidth(0, m_ColumnWidth);
		ImGui::Text(m_Label.c_str());
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
			m_Value.x = m_ResetValue;
		}

		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &m_Value.x, m_Step, m_MinValue, m_MaxValue, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.75f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));

		if (ImGui::Button("Y", buttonSize))
		{
			m_Value.y = m_ResetValue;
		}

		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &m_Value.y, m_Step, m_MinValue, m_MaxValue, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.75f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));

		if (ImGui::Button("Z", buttonSize))
		{
			m_Value.z = m_ResetValue;
		}

		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &m_Value.z, m_Step, m_MinValue, m_MaxValue, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PopStyleVar();
		ImGui::Columns(1);

		ImGui::PopID();
	}


}
