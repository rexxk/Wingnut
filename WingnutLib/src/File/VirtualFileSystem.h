#pragma once



namespace Wingnut
{


	enum class FileItemType
	{
		Model,
		Texture,
		Scene,
	};


	struct FileSystemItem
	{
		std::string Name;

		FileItemType Type;

		void* ItemLink = nullptr;

		std::vector<uint8_t> Data;
		uint32_t DataSize = 0;
	};

	struct FileSystemDirectory
	{
		std::string Name;

		uint32_t DataSize = 0;

		FileSystemDirectory* Parent = nullptr;

		std::vector<FileSystemItem> Files;
		std::vector<FileSystemDirectory> Subdirectories;
	};


	class VirtualFileSystem
	{
	public:
		VirtualFileSystem();
		~VirtualFileSystem();

		static void LoadFileFromResource();
		static void LoadFileFromDisk(const std::string& filepath, FileItemType type);

		static void AddFile(const std::string& filepath, const std::vector<uint8_t>& data, uint32_t dataSize, FileItemType type);
		static bool FindFile(const std::string& filepath);
		static FileSystemItem* GetItem(const std::string& filepath);

		static void SetItemLink(const std::string& filepath, void* link);

		static void PrintStructure();
	
		static FileSystemDirectory* GetDirectory(const std::string& filepath);
		static FileSystemDirectory& GetRootDirectory() { return s_Instance->m_RootDirectory; }
		
	private:
		static void AddDirectories(FileSystemDirectory& directory, const std::vector<std::string>& paths, uint32_t levels, uint32_t actualLevel = 0);

		static bool FindDirectory(const FileSystemDirectory& directory, const std::vector<std::string>& paths, uint32_t levels, FileSystemDirectory** returnDirectory = nullptr, uint32_t actualLevel = 0);

		static void PrintDirectoryStructure(const FileSystemDirectory& directory, std::string directoryName = "/", uint32_t level = 0);

	private:
		inline static VirtualFileSystem* s_Instance = nullptr;

		uint32_t m_TotalDataSize = 0;

		FileSystemDirectory m_RootDirectory;
	};



}
