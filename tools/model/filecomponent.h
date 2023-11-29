#pragma once
#include <fstream>
#include <string>

class FileComponent
{
public:
	FileComponent(const std::string &outputPath, unsigned index, std::streamoff fileBeg);
	virtual std::streamoff Load(std::ifstream &file) = 0;
	virtual void ToObj() = 0;

protected:
	void FileSeekBeg(std::ifstream &file, std::streamoff offset = 0);

	std::string m_outputPath;
	unsigned m_index;
	std::streamoff m_fileBeg;
};

