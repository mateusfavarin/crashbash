#pragma once
#include <filesystem>
#include <fstream>
#include <vector>
#include "primitives.h"
#include "mesh.h"

namespace fs = std::filesystem;

class MDL
{
public:
	MDL(fs::path mdlPath);
	void ToObj();

private:
	fs::path m_path;
	std::ifstream m_file;

	std::string m_name;
	std::string m_outputPath;

	MDLHeader m_header;
	std::vector<Mesh *> m_meshList;

	void ReadHeader();
	void LoadMeshes();
};