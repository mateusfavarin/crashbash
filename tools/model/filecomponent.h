#pragma once
#include <fstream>
#include <string>
#include <iostream>

class FileComponent
{
public:
	FileComponent(const std::string &outputPath, unsigned index, std::streamoff fileBeg);
	virtual std::streamoff Load(std::ifstream &file) = 0;
	virtual void ToObj() = 0;

protected:
	void FileSeekRelative(std::ifstream &file, std::streamoff offset = 0);
	void FileSeekAbsolute(std::ifstream &file, std::streamoff position);

	std::string m_outputPath;
	unsigned m_index;
	std::streamoff m_fileBeg;
};

