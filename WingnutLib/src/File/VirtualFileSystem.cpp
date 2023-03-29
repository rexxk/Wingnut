#include "wingnut_pch.h"
#include "VirtualFileSystem.h"

#include "Utils/StringUtils.h"


namespace Wingnut
{

	VirtualFileSystem::VirtualFileSystem()
	{
		if (s_Instance == nullptr)
		{
			s_Instance = this;
		}

		m_RootDirectory.Name = "/";

	}

	VirtualFileSystem::~VirtualFileSystem()
	{

	}

	void VirtualFileSystem::AddFile(const std::string& filepath, const std::vector<uint8_t>& data, uint32_t dataSize, FileItemType type, bool systemFile)
	{
		std::vector<std::string> tokens = Tokenize(filepath, '/');

		if (tokens.empty())
		{
			return;
		}
		if (tokens.size() == 1)
		{
			s_Instance->m_RootDirectory.Files.emplace_back(tokens[tokens.size() - 1]);
			return;
		}

		if (!FindDirectory(s_Instance->m_RootDirectory, tokens, (uint32_t)tokens.size() - 1))
		{
			AddDirectories(s_Instance->m_RootDirectory, tokens, (uint32_t)tokens.size() - 1);
		}

		FileSystemDirectory* workingDirectory;

		if (FindDirectory(s_Instance->m_RootDirectory, tokens, (uint32_t)tokens.size() - 1, &workingDirectory))
		{
			FileSystemItem newFile;
			newFile.Name = tokens[tokens.size() - 1];
			newFile.Data = data;
			newFile.DataSize = dataSize;
			newFile.Type = type;

			newFile.SystemFile = systemFile;

//			workingDirectory->Files.emplace_back(tokens[tokens.size() - 1]);
			workingDirectory->Files.emplace_back(newFile);

			workingDirectory->DataSize += dataSize;

			s_Instance->m_TotalDataSize += dataSize;
		}

	}

	void VirtualFileSystem::LoadFileFromResource()
	{

	}

	bool VirtualFileSystem::LoadFileFromDisk(const std::string& filepath, FileItemType type, bool systemFile)
	{
		std::string assetPath = ConvertFilePathToAssetPath(filepath);

		if (assetPath == "")
		{
			assetPath = filepath;
		}

		if (FindFile(assetPath))
		{
			LOG_CORE_TRACE("[VFS] File {} is already loaded", assetPath);
			return false;
		}

		std::ifstream file(filepath, std::ios::in | std::ios::binary);

		if (!file.is_open())
		{
			LOG_CORE_TRACE("[VFS] Unable to open file {} for loading", assetPath);
			return false;
		}

		file.seekg(0, file.end);
		size_t fileSize = file.tellg();
		file.seekg(0, file.beg);

		std::vector<uint8_t> fileData(fileSize);

		file.read((char*)fileData.data(), fileSize);

		file.close();

		AddFile(assetPath, fileData, (uint32_t)fileSize, type, systemFile);

		return true;
	}

	bool VirtualFileSystem::FindFile(const std::string& filepath)
	{
		std::vector<std::string> tokens = Tokenize(filepath, '/');

		FileSystemDirectory* workingDirectory;

		if (FindDirectory(s_Instance->m_RootDirectory, tokens, (uint32_t)tokens.size() - 1, &workingDirectory))
		{
			for (auto& file : workingDirectory->Files)
			{
				if (file.Name == tokens[tokens.size() - 1])
				{
					return true;
				}
			}
		}

		return false;
	}

	FileSystemItem* VirtualFileSystem::GetItem(const std::string& filepath)
	{
		std::vector<std::string> tokens = Tokenize(filepath, '/');

		FileSystemDirectory* workingDirectory;

		if (FindDirectory(s_Instance->m_RootDirectory, tokens, (uint32_t)tokens.size() - 1, &workingDirectory))
		{
			for (auto& file : workingDirectory->Files)
			{
				if (file.Name == tokens[tokens.size() - 1])
				{
					return &file;
				}
			}
		}

		return nullptr;
	}

	void VirtualFileSystem::SetItemLink(const std::string& filepath, void* link)
	{
		FileSystemItem* item = GetItem(filepath);

		if (item != nullptr)
		{
			item->ItemLink = link;
		}
	}

	void VirtualFileSystem::AddDirectories(FileSystemDirectory& directory, const std::vector<std::string>& paths, uint32_t levels, uint32_t actualLevel)
	{
		FileSystemDirectory* returnDirectory;

		for (uint32_t i = 0; i < levels; i++)
		{
			if (!FindDirectory(directory, paths, i + 1, &returnDirectory))
			{

				FileSystemDirectory newDirectory;
				newDirectory.Name = paths[i];

				newDirectory.Parent = returnDirectory;

//				returnDirectory->Subdirectories.emplace_back(paths[i]);
				returnDirectory->Subdirectories.emplace_back(newDirectory);
			}
		}
	}

	bool VirtualFileSystem::FindDirectory(const FileSystemDirectory& directory, const std::vector<std::string>& paths, uint32_t levels, FileSystemDirectory** returnDirectory, uint32_t actualLevel)
	{
		if (actualLevel == levels)
		{
			return true;
		}

		if (returnDirectory != nullptr)
		{
			*returnDirectory = (FileSystemDirectory*)&directory;
		}

		for (auto& subdirectory : directory.Subdirectories)
		{

			if (subdirectory.Name == paths[actualLevel])
			{
				if (returnDirectory != nullptr)
				{
					*returnDirectory = (FileSystemDirectory*)&subdirectory;
				}

				return FindDirectory(subdirectory, paths, levels, returnDirectory, ++actualLevel);
			}
		}

		return false;
	}

	FileSystemDirectory* VirtualFileSystem::GetDirectory(const std::string& filepath)
	{
		std::vector<std::string> tokens = Tokenize(filepath, '/');

		FileSystemDirectory* workingDirectory;

		if (FindDirectory(s_Instance->m_RootDirectory, tokens, (uint32_t)tokens.size() - 1, &workingDirectory))
		{
			return workingDirectory;
		}

		return nullptr;
	}

	void VirtualFileSystem::PrintStructure()
	{
		PrintDirectoryStructure(s_Instance->m_RootDirectory);

		LOG_CORE_TRACE("Total file size: {} kb", (uint32_t)(s_Instance->m_TotalDataSize >> 10) + 1);
	}

	void VirtualFileSystem::PrintDirectoryStructure(const FileSystemDirectory& directory, std::string directoryName, uint32_t level)
	{	
		LOG_CORE_TRACE("{}", directoryName);

		level++;

		for (auto& file : directory.Files)
		{
			std::string fileString = "";

			for (uint32_t i = 0; i < (level - 1); i++)
			{
				fileString += ' ';
			}

			fileString += file.Name;
			LOG_CORE_TRACE(" {}   [ {} kB ]", fileString, ((uint32_t)file.DataSize >> 10) + 1);
		}

		for (auto& subdirectory : directory.Subdirectories)
		{
			PrintDirectoryStructure(subdirectory, directoryName + subdirectory.Name + '/', level);
		}

	}

}
