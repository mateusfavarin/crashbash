#include <fstream>
#include "mesh.h"

Mesh::Mesh(const std::string & outputPath, unsigned index, std::ifstream &file)
{
	m_index = index;
	m_outputPath = outputPath + "mesh_" + std::to_string(m_index) + ".obj";
	LoadMesh(file);
}

void Mesh::LoadMesh(std::ifstream &file)
{
	std::streamoff meshBeg = file.tellg();
	file.read((char *) &m_header, sizeof(m_header));
	file.seekg(meshBeg + offsetof(MeshHeader, vertexEncodeOffset) + m_header.vertexEncodeOffset, std::ios::beg);
	while (true)
	{
		VertexEncode ve;
		file.read((char *) &ve, sizeof(ve));
		if (ve.count == 0xFF)
		{
			break;
		}
		m_vertexEncodeList.push_back(ve);
	}

	file.seekg(meshBeg + offsetof(MeshHeader, vertexListOffset) + m_header.vertexListOffset);
	file.read((char *) &m_vertexHeader, sizeof(m_vertexHeader));
	m_vertexHeader.bbox.min.InvertCoords();
	m_vertexHeader.bbox.max.InvertCoords();

	Vertex v[TRI_VERTEX_COUNT];
	bool flipTri;
	int vertexCount = 0;
	for (VertexEncode ve : m_vertexEncodeList)
	{
		flipTri = (ve.flag & 0x8) == 0;

		file.read((char *) &v[0], sizeof(Vertex));
		v[0].InvertCoords();
		file.read((char *) &v[1], sizeof(Vertex));
		v[1].InvertCoords();
		for (unsigned i = 0; i < ve.count; i++)
		{
			file.read((char *) &v[2], sizeof(Vertex));
			v[2].InvertCoords();
			m_triList.emplace_back(Triangle(v, vertexCount + 1, flipTri));

			v[0] = v[1];
			v[1] = v[2];
			vertexCount += 3;
			flipTri = !flipTri;
		}
	}
}

void Mesh::ToObj()
{
	std::ofstream obj(m_outputPath, std::ios::out);
	for (Triangle &tri : m_triList)
	{
		obj << tri;
	}
}