#pragma once
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

class FileManager
{
public:
	virtual void Export() = 0;

protected:
	fs::path m_path;
	std::ifstream m_file;
	std::streamoff m_fileBeg;
	std::string m_name;
	std::string m_fileType;
	std::string m_outputPath;

	FileManager(fs::path path, std::string fileType);
	void FileSeekRelative(std::streamoff offset = 0);
	void FileSeekAbsolute(std::streamoff position);
};
