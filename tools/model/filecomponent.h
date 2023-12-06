#pragma once
#include <fstream>
#include <string>
#include <iostream>

class FileComponent
{
public:
	const std::string& GetOutputPath() const;
	virtual std::streamoff Load(std::ifstream &file) = 0;
	virtual void Export() = 0;

protected:
	FileComponent() {};
	FileComponent(const std::string &outputPath, unsigned index, std::streamoff fileBeg);
	std::string m_outputPath;
	unsigned m_index;
	std::streamoff m_fileBeg;

	void FileSeekRelative(std::ifstream &file, std::streamoff offset = 0);
	void FileSeekAbsolute(std::ifstream &file, std::streamoff position);
	void FileJump(std::ifstream &file, std::streamoff offset);
};
