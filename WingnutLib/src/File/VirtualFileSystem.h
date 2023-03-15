#pragma once



namespace Wingnut
{



	struct FileSystemItem
	{
		std::string Name;

	};

	struct FileSystemDirectory
	{
		std::string Name;


		std::vector<FileSystemItem> Files;
		std::vector<FileSystemDirectory> Subdirectories;
	};


	class VirtualFileSystem
	{
	public:
		VirtualFileSystem();
		~VirtualFileSystem();

		void AddFile(const std::string& filepath);

		void PrintDirectoryStructure(const FileSystemDirectory& directory, std::string directoryName = "/", uint32_t level = 0);

		FileSystemDirectory& GetRootDirectory() { return m_RootDirectory; }

	private:
		void AddDirectories(FileSystemDirectory& directory, const std::vector<std::string>& paths, uint32_t levels, uint32_t actualLevel = 0);

		bool FindDirectory(const FileSystemDirectory& directory, const std::vector<std::string>& paths, uint32_t levels, FileSystemDirectory** returnDirectory = nullptr, uint32_t actualLevel = 0);

	private:

		FileSystemDirectory m_RootDirectory;
	};



}
