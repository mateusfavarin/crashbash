#pragma once
#include <vector>
#include "primitives.h"
#include "filecomponent.h"

class Mesh : public FileComponent
{
public:
	Mesh(const std::string &outputPath, const std::string &meshType, unsigned index, std::streamoff fileBeg, std::streamoff animIndexPos = 0, std::streamoff animDataPos = 0);
	std::streamoff Load(std::ifstream &file) override;
	void ToObj() override;

private:
	std::streamoff m_animIndexPos;
	std::streamoff m_animDataPos;
	bool m_isAnimated;

	MeshHeader m_header;
	VertexHeader m_vertexHeader;
	unsigned m_numVertexes;
	std::vector<uint16_t> m_animVertexIndexList;
	std::vector<Vertex> m_vertexList;
	std::vector<Triangle> m_triList;
	std::vector<VertexCompression> m_vertexCompressionList;

	std::streamoff LoadHeader(std::ifstream &file);
	std::streamoff LoadAnimVertexIndex(std::ifstream &file);
	std::streamoff LoadVertexes(std::ifstream &file);
	void LoadAnimVertexes(std::ifstream &file);
	void VertexesToTriangles();
	std::streamoff LoadVertexCompression(std::ifstream &file);
};