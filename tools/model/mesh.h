#pragma once
#include <vector>
#include "primitives.h"
#include "filecomponent.h"
#include "tex.h"

class Mesh : public FileComponent
{
public:
	Mesh(const std::string &outputPath, const std::string &meshType, const std::string &name, unsigned index, std::streamoff fileBeg, std::streamoff vcolorDataPos, std::streamoff uvDataPos, std::streamoff animIndexPos = 0, std::streamoff animDataPos = 0);
	std::streamoff Load(std::ifstream &file) override;
	void ConvertVertexesToTriangles(std::ifstream &file, Tex &tex);
	void Export() override;

private:
	std::string m_name;
	std::streamoff m_animIndexPos;
	std::streamoff m_animDataPos;
	std::streamoff m_vcolorDataPos;
	std::streamoff m_uvDataPos;
	bool m_isAnimated;

	MeshHeader m_header;
	VertexHeader m_vertexHeader;
	unsigned m_numVertexes;
	unsigned m_numTriangles;
	std::vector<uint16_t> m_animVertexIndexList;
	std::vector<uint16_t> m_vcolorFlagList;
	std::vector<uint16_t> m_uvIndexList;
	std::vector<Texinfo> m_texinfoList;
	std::vector<Vertex> m_vertexList;
	std::vector<Triangle> m_triList;
	std::vector<VertexCompression> m_vertexCompressionList;

	std::streamoff LoadHeader(std::ifstream &file);
	std::streamoff LoadVcolorFlags(std::ifstream &file);
	std::streamoff LoadUvIndexes(std::ifstream &file);
	std::streamoff LoadTexinfos(std::ifstream &file);
	std::streamoff LoadAnimVertexIndex(std::ifstream &file);
	std::streamoff LoadVertexes(std::ifstream &file);
	void LoadAnimVertexes(std::ifstream &file);
	uint16_t LoadVcolor(std::ifstream &file, Color * color, unsigned triCount);
	void LoadUV(std::ifstream &file, UV * uv, unsigned triCount);
	std::streamoff LoadVertexCompression(std::ifstream &file);
};