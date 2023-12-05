#include "filemanager.h"

const std::string& FileManager::GetOutputPath() const
{
	return m_outputPath;
}

FileManager::FileManager(fs::path path, std::string fileType)
{
	m_path = path;
	m_file = std::ifstream(m_path, std::ios::in | std::ios::binary);
	m_fileBeg = m_file.tellg();
	m_name = m_path.stem().string();
	m_fileType = fileType;
	m_outputPath = m_name + "_" + m_fileType + "/";
	fs::create_directory(m_outputPath);
}

void FileManager::FileSeekRelative(std::streamoff offset)
{
	m_file.seekg(m_fileBeg + offset, std::ios::beg);
}

void FileManager::FileSeekAbsolute(std::streamoff position)
{
	m_file.seekg(position, std::ios::beg);
}