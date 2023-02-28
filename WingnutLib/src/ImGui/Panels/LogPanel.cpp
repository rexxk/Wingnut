#include "wingnut_pch.h"
#include "LogPanel.h"

#include <imgui.h>


namespace Wingnut
{

	LogPanel::LogPanel()
	{
		if (s_Instance == nullptr)
		{
			s_Instance = this;
		}
	}

	LogPanel::~LogPanel()
	{

	}

	void LogPanel::Draw()
	{
		ImGui::Begin("Log");

		for (auto& string : m_LogStrings)
		{
			ImGui::Text("%s", string.c_str());
		}

		if (m_ScrollToBottom)
		{
			ImGui::SetScrollHereY(1.0f);

			m_ScrollToBottom = false;
		}

		ImGui::End();
	}

	void LogPanel::AddLogText(const std::string& logText)
	{
		s_Instance->m_LogStrings.emplace_back(logText);

		s_Instance->m_ScrollToBottom = true;
	}

}
