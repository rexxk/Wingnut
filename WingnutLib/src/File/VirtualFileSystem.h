#pragma once



namespace Wingnut
{



	struct FileSystemItem
	{
		std::string Name;

		char* Data = nullptr;
		uint32_t DataSize = 0;
	};

	struct FileSystemDirectory
	{
		std::string Name;

		uint32_t DataSize = 0;

		std::vector<FileSystemItem> Files;
		std::vector<FileSystemDirectory> Subdirectories;
	};


	class VirtualFileSystem
	{
	public:
		VirtualFileSystem();
		~VirtualFileSystem();

		static void AddFile(const std::string& filepath);

		static bool FindFile(const std::string& filepath);

		static void PrintDirectoryStructure(const FileSystemDirectory& directory, std::string directoryName = "/", uint32_t level = 0);
		
		static FileSystemDirectory& GetRootDirectory() { return s_Instance->m_RootDirectory; }

	private:
		static void AddDirectories(FileSystemDirectory& directory, const std::vector<std::string>& paths, uint32_t levels, uint32_t actualLevel = 0);

		static bool FindDirectory(const FileSystemDirectory& directory, const std::vector<std::string>& paths, uint32_t levels, FileSystemDirectory** returnDirectory = nullptr, uint32_t actualLevel = 0);

	private:
		inline static VirtualFileSystem* s_Instance = nullptr;

		uint32_t TotalDataSize = 0;

		FileSystemDirectory m_RootDirectory;
	};



}
