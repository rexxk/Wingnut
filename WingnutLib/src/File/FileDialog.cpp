#include "wingnut_pch.h"
#include "FileDialog.h"

#include "Core/Application.h"

#include "Utils/StringUtils.h"


#ifdef _WIN32
#include <commdlg.h>
#endif

namespace Wingnut
{

	std::string FileDialog::Load(const TCHAR* filter, const std::string& path)
	{
		std::string filename;

#ifdef _WIN32

		OPENFILENAME ofn = {};
		wchar_t file[MAX_PATH] = {};
		wchar_t initialPath[MAX_PATH];

		std::mbstowcs(initialPath, path.c_str(), path.size());

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = (HWND)Application::Get().GetWindow()->WindowHandle();
		ofn.hInstance = GetModuleHandle(NULL);
		ofn.lpstrTitle = L"Open...";
		ofn.lpstrFile = file;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrInitialDir = initialPath;
		ofn.lpstrFilter = filter;
		ofn.Flags = OFN_NOCHANGEDIR;

		if (GetOpenFileName(&ofn))
		{
			filename.resize(MAX_PATH);
			std::wcstombs(filename.data(), file, MAX_PATH);
		}

#endif

		return ConvertFilePath(filename);
	}

	std::string FileDialog::Save(const TCHAR* filter, const std::string& path, const TCHAR* extension)
	{
		std::string filename;

#ifdef _WIN32

		OPENFILENAME ofn = {};
		wchar_t file[MAX_PATH] = {};
		wchar_t initialPath[MAX_PATH];

		std::mbstowcs(initialPath, path.c_str(), path.size());

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = (HWND)Application::Get().GetWindow()->WindowHandle();
		ofn.hInstance = GetModuleHandle(NULL);
		ofn.lpstrTitle = L"Save...";
		ofn.lpstrFile = file;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrInitialDir = initialPath;
		ofn.lpstrFilter = filter;
		ofn.lpstrDefExt = extension;
		ofn.Flags = OFN_NOCHANGEDIR;

		if (GetSaveFileName(&ofn))
		{
			filename.resize(MAX_PATH);
			std::wcstombs(filename.data(), file, MAX_PATH);
		}


#endif

		return ConvertFilePath(filename);
	}

}
