#pragma once


namespace Wingnut
{

	class LogPanel
	{
	public:
		LogPanel();
		~LogPanel();

		void Draw();

		static void AddLogText(const std::string& logText);

	private:
		inline static LogPanel* s_Instance = nullptr;

		bool m_ScrollToBottom = false;

		std::vector<std::string> m_LogStrings;
	};

}
