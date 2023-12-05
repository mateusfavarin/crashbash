#include "filecomponent.h"

const std::string& FileComponent::GetOutputPath() const
{
	return m_outputPath;
}

FileComponent::FileComponent(const std::string &outputPath, unsigned index, std::streamoff fileBeg)
{
	m_fileBeg = fileBeg;
	m_index = index;
	m_outputPath = outputPath;
}

void FileComponent::FileSeekRelative(std::ifstream &file, std::streamoff offset)
{
	file.seekg(m_fileBeg + offset, std::ios::beg);
}

void FileComponent::FileSeekAbsolute(std::ifstream &file, std::streamoff position)
{
	file.seekg(position, std::ios::beg);
}

void FileComponent::FileJump(std::ifstream & file, std::streamoff offset)
{
	file.seekg(offset, std::ios::cur);
}
