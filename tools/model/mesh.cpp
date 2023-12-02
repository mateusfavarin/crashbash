#include "mesh.h"

Mesh::Mesh(const std::string &outputPath, const std::string &meshType, unsigned index, std::streamoff fileBeg, std::streamoff vcolorDataPos, std::streamoff animIndexPos, std::streamoff animDataPos)
	: FileComponent(outputPath + meshType + "_" + std::to_string(index) + ".obj", index, fileBeg)
{
	m_vcolorDataPos = vcolorDataPos;
	m_animIndexPos = animIndexPos;
	m_animDataPos = animDataPos;
	m_isAnimated = (m_animIndexPos != 0) && (m_animDataPos != 0);
	m_numVertexes = 0;
	m_numTriangles = 0;
	m_header = {};
	m_vertexHeader = {};
};

std::streamoff Mesh::LoadHeader(std::ifstream &file)
{
	FileSeekRelative(file);
	file.read((char *) &m_header, sizeof(m_header));
	return file.tellg();
}

std::streamoff Mesh::LoadVertexCompression(std::ifstream &file)
{
	FileSeekRelative(file, offsetof(MeshHeader, vertexCompressionOffset) + m_header.vertexCompressionOffset);
	unsigned numVertexes = 0;
	while (true)
	{
		VertexCompression ve;
		file.read((char *) &ve, sizeof(ve));
		if (ve.count == 0xFF)
		{
			break;
		}
		numVertexes += 2 + ve.count;
		m_numTriangles += ve.count;
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

std::streamoff Mesh::LoadVcolorFlags(std::ifstream &file)
{
	FileSeekRelative(file, offsetof(MeshHeader, vcolorFlagsOffset) + m_header.vcolorFlagsOffset);
	uint16_t vcolorFlag;
	for (unsigned i = 0; i < m_numTriangles; i++)
	{
		file.read((char *) &vcolorFlag, sizeof(vcolorFlag));
		m_vcolorFlagList.push_back(vcolorFlag);
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

void Mesh::VertexesToTriangles(std::ifstream &file)
{
	Vertex v[TRI_VERTEX_COUNT];
	Color color[TRI_VERTEX_COUNT];
	bool flipTri;
	bool isTextured;
	unsigned fileVertexCount = 0;
	unsigned triCount = 0;
	unsigned k = 0;
	for (VertexCompression ve : m_vertexCompressionList)
	{
		flipTri = (ve.flag & 0x8) == 0;
		isTextured = (ve.flag & VertexEncodeFlag::Untextured) == 0;
		v[0] = m_vertexList[k++];
		v[1] = m_vertexList[k++];
		for (unsigned i = 0; i < ve.count; i++)
		{
			v[2] = m_vertexList[k++];

			uint16_t vcolorFlag = m_vcolorFlagList[triCount++];
			std::streamoff vcolorIndex = (vcolorFlag & VcolorFlag::IDBits) * sizeof(Color);
			FileSeekAbsolute(file, m_vcolorDataPos + vcolorIndex);
			file.read((char *) &color, sizeof(color));
			Triangle tri = Triangle(v, fileVertexCount + 1, color, isTextured, flipTri);
			if (vcolorFlag & VcolorFlag::SemiTransparent)
			{
				tri.SetSemiTransparencyMode(color[0].g);
			}
			m_triList.push_back(tri);

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
	LoadVcolorFlags(file);
	if (m_isAnimated)
	{
		LoadAnimVertexIndex(file);
		LoadAnimVertexes(file);
	}
	else
	{
		LoadVertexes(file);
	}
	VertexesToTriangles(file);
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