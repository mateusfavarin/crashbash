#include "mesh.h"

Mesh::Mesh(const std::string &outputPath, const std::string &meshType, unsigned index, std::streamoff fileBeg, std::streamoff animIndexPos, std::streamoff animDataPos)
	: FileComponent(outputPath + meshType + "_" + std::to_string(index) + ".obj", index, fileBeg)
{
	m_animIndexPos = animIndexPos;
	m_animDataPos = animDataPos;
	m_isAnimated = (m_animIndexPos != 0) && (m_animDataPos != 0);
	m_numVertexes = 0;
	m_header = {};
	m_vertexHeader = {};
};

std::streamoff Mesh::LoadHeader(std::ifstream &file)
{
	FileSeekRelative(file);
	file.read((char *)&m_header, sizeof(m_header));
	return file.tellg();
}

std::streamoff Mesh::LoadVertexCompression(std::ifstream &file)
{
	FileSeekRelative(file, offsetof(MeshHeader, vertexCompressionOffset) + m_header.vertexCompressionOffset);
	unsigned numVertexes = 0;
	while (true)
	{
		VertexCompression ve;
		file.read((char *)&ve, sizeof(ve));
		if (ve.count == 0xFF)
		{
			break;
		}
		numVertexes += 2 + ve.count;
		m_vertexCompressionList.push_back(ve);
	}
	m_numVertexes = numVertexes;
	return file.tellg();
}

std::streamoff Mesh::LoadAnimVertexIndex(std::ifstream &file)
{
	FileSeekAbsolute(file, m_animIndexPos);
	file.read((char *)&m_vertexHeader, sizeof(m_vertexHeader));
	m_vertexHeader.bbox.min.InvertCoords();
	m_vertexHeader.bbox.max.InvertCoords();

	uint16_t index;
	for (unsigned i = 0; i < m_numVertexes; i++)
	{
		file.read((char *) &index, sizeof(index));
		m_animVertexIndexList.push_back((index >> 2) * sizeof(Point));
	}
	return file.tellg();
}

std::streamoff Mesh::LoadVertexes(std::ifstream &file)
{
	FileSeekRelative(file, offsetof(MeshHeader, vertexListOffset) + m_header.vertexListOffset);
	file.read((char *)&m_vertexHeader, sizeof(m_vertexHeader));
	m_vertexHeader.bbox.min.InvertCoords();
	m_vertexHeader.bbox.max.InvertCoords();

	Vertex v;
	for (unsigned i = 0; i < m_numVertexes; i++)
	{
		file.read((char *) &v, sizeof(v));
		v.InvertCoords();
		m_vertexList.push_back(v);
	}

	return file.tellg();
}

void Mesh::LoadAnimVertexes(std::ifstream &file)
{
	Vertex v;
	for (uint16_t &index : m_animVertexIndexList)
	{
		FileSeekAbsolute(file, m_animDataPos + index);
		file.read((char *) &v, sizeof(v));
		v.InvertCoords();
		m_vertexList.push_back(v);
	}
}

void Mesh::VertexesToTriangles()
{
	Vertex v[3];
	bool flipTri;
	unsigned fileVertexCount = 0;
	unsigned k = 0;
	for (VertexCompression ve : m_vertexCompressionList)
	{
		flipTri = (ve.flag & 0x8) == 0;
		v[0] = m_vertexList[k++];
		v[1] = m_vertexList[k++];
		for (unsigned i = 0; i < ve.count; i++)
		{
			v[2] = m_vertexList[k++];
			m_triList.emplace_back(Triangle(v, fileVertexCount + 1, flipTri));

			v[0] = v[1];
			v[1] = v[2];
			fileVertexCount += 3;
			flipTri = !flipTri;
		}
	}
}

std::streamoff Mesh::Load(std::ifstream &file)
{
	std::streamoff headerEnd = LoadHeader(file);
	LoadVertexCompression(file);
	if (m_isAnimated)
	{
		LoadAnimVertexIndex(file);
		LoadAnimVertexes(file);
	}
	else
	{
		LoadVertexes(file);
	}
	VertexesToTriangles();
	return headerEnd;
}

void Mesh::ToObj()
{
	std::ofstream obj(m_outputPath, std::ios::out);
	for (Triangle &tri : m_triList)
	{
		obj << tri;
	}
}