#include <fstream>
#include "mesh.h"

Mesh::Mesh(const std::string & outputPath, unsigned index, std::ifstream * file)
{
	m_file = file;
	m_fileOffset = file->tellg();
	m_index = index;
	m_outputPath = outputPath + "mesh_" + std::to_string(m_index) + ".obj";
	LoadMesh();
}

void Mesh::LoadMesh()
{
	m_file->read((char *) &m_header, sizeof(m_header));

	m_file->seekg(m_fileOffset + offsetof(MeshHeader, vertexEncodeOffset) + m_header.vertexEncodeOffset, std::ios::beg);
	while (true)
	{
		VertexEncode ve;
		m_file->read((char *) &ve, sizeof(ve));
		if (ve.count == 0xFF)
		{
			break;
		}
		m_vertexEncodeList.push_back(ve);
	}

	m_file->seekg(m_fileOffset + offsetof(MeshHeader, vertexListOffset) + m_header.vertexListOffset);
	m_file->read((char *) &m_vertexHeader, sizeof(m_vertexHeader));
	m_vertexHeader.bbox.min.InvertCoords();
	m_vertexHeader.bbox.max.InvertCoords();

	Vertex v[TRI_VERTEX_COUNT];
	bool flipTri;
	int vertexCount = 0;
	for (VertexEncode ve : m_vertexEncodeList)
	{
		flipTri = (ve.flag & 0x8) == 0;

		m_file->read((char *) &v[0], sizeof(Vertex));
		v[0].InvertCoords();
		m_file->read((char *) &v[1], sizeof(Vertex));
		v[1].InvertCoords();
		for (unsigned i = 0; i < ve.count; i++)
		{
			m_file->read((char *) &v[2], sizeof(Vertex));
			v[2].InvertCoords();

			Triangle * pTri = new Triangle(v, vertexCount + 1, flipTri);
			m_triList.push_back(pTri);

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
	for (Triangle * pTri : m_triList)
	{
		obj << *pTri;
	}
}