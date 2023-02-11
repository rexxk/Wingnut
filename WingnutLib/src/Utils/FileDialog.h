#pragma once


namespace Wingnut
{

	class OpenFileDialog
	{
	public:
		static 	std::string Open(const TCHAR* filter, const std::string& path = "");
	};

}
