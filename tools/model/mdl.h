#pragma once
#include <filesystem>
#include <fstream>
#include <vector>
#include "primitives.h"
#include "mesh.h"
#include "anim.h"

namespace fs = std::filesystem;

class MDL
{
public:
	MDL(fs::path mdlPath);
	void ToObj();

private:
	fs::path m_path;
	std::ifstream m_file;
	std::streamoff m_fileBeg;

	std::string m_name;
	std::string m_outputPath;

	MDLHeader m_header;
	std::vector<Mesh> m_meshList;
	std::vector<Anim> m_animList;

	inline void FileSeekRelative(std::streamoff offset = 0);
	inline void FileSeekAbsolute(std::streamoff position);
	void ReadHeader();
	void LoadMeshes();
	void LoadAnims();
};