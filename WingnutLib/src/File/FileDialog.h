#pragma once


namespace Wingnut
{

	class FileDialog
	{
	public:
		static std::string Load(const TCHAR* filter, const std::string& path = "");
		static std::string Save(const TCHAR* filter, const std::string& path = "", const TCHAR* extension = L".wfile");
	};

}
