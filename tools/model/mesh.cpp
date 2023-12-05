#include "mesh.h"

Mesh::Mesh(const std::string &outputPath, const std::string &meshType, const std::string &name, unsigned index, std::streamoff fileBeg, std::streamoff vcolorDataPos, std::streamoff uvDataPos, std::streamoff animIndexPos, std::streamoff animDataPos)
	: FileComponent(outputPath + meshType + "_" + std::to_string(index), index, fileBeg)
{
	m_name = name;
	m_vcolorDataPos = vcolorDataPos;
	m_uvDataPos = uvDataPos;
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

std::streamoff Mesh::LoadUvIndexes(std::ifstream &file)
{
	FileSeekRelative(file, offsetof(MeshHeader, uvIndexOffset) + m_header.uvIndexOffset);
	uint16_t uvIndex;
	for (unsigned i = 0; i < m_numTriangles; i++)
	{
		file.read((char *) &uvIndex, sizeof(uvIndex));
		m_uvIndexList.push_back(uvIndex);
	}
	return file.tellg();
}

std::streamoff Mesh::LoadTexinfos(std::ifstream &file)
{
	FileSeekRelative(file, offsetof(MeshHeader, texinfoOffset) + m_header.texinfoOffset);
	Texinfo texinfo;
	unsigned trianglesRead = 0;
	while (trianglesRead < m_numTriangles)
	{
		file.read((char *) &texinfo, sizeof(texinfo));
		texinfo.triCount++;
		m_texinfoList.push_back(texinfo);
		trianglesRead += texinfo.triCount;
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

uint16_t Mesh::LoadVcolor(std::ifstream &file, Color * color, unsigned triCount)
{
	uint16_t vcolorFlag = m_vcolorFlagList[triCount];
	std::streamoff vcolorOffset = (vcolorFlag & VcolorFlag::IDBits) * sizeof(Color);
	FileSeekAbsolute(file, m_vcolorDataPos + vcolorOffset);
	file.read((char *) color, sizeof(Color) * TRI_VERTEX_COUNT);
	return vcolorFlag;
}

void Mesh::LoadUV(std::ifstream &file, UV * uv, unsigned triCount)
{
	uint16_t uvIndex = m_uvIndexList[triCount];
	std::streamoff uvOffset = uvIndex * sizeof(UV);
	FileSeekAbsolute(file, m_uvDataPos + uvOffset);
	file.read((char *) uv, sizeof(UV) * TRI_VERTEX_COUNT);
}

void Mesh::ConvertVertexesToTriangles(std::ifstream &file, Tex &tex)
{
	Vertex v[TRI_VERTEX_COUNT];
	Color color[TRI_VERTEX_COUNT];
	UV uv[TRI_VERTEX_COUNT];
	bool flipTri;
	bool isTextured;
	unsigned fileVertexCount = 0;
	unsigned fileUVCount = 0;
	unsigned triCount = 0;
	unsigned trisLinkedTex = 0;
	unsigned texinfoIndex = 0;
	unsigned texIndex = 0;
	unsigned texUnkFlag = 0;
	unsigned k = 0;
	for (VertexCompression ve : m_vertexCompressionList)
	{
		flipTri = (ve.flag & VertexEncodeFlag::FlipTriangle) == 0;
		isTextured = (ve.flag & VertexEncodeFlag::Untextured) == 0;
		v[0] = m_vertexList[k++];
		v[1] = m_vertexList[k++];
		for (unsigned i = 0; i < ve.count; i++)
		{
			v[2] = m_vertexList[k++];

			uint16_t vcolorFlag = LoadVcolor(file, color, triCount);
			if (isTextured)
			{
				LoadUV(file, uv, triCount);
			}
			if (trisLinkedTex == 0)
			{
				Texinfo texinfo = m_texinfoList[texinfoIndex++];
				trisLinkedTex = texinfo.triCount;
				texIndex = texinfo.index;
				texUnkFlag = texinfo.unkFlag;
			}
			Triangle tri = Triangle(v, fileVertexCount + 1, fileUVCount + 1, color, uv, tex.GetImage(texIndex).GetWidth(), tex.GetImage(texIndex).GetHeight(), texIndex, isTextured, flipTri);
			if (isTextured)
			{
				fileUVCount += TRI_VERTEX_COUNT;
				if (vcolorFlag & VcolorFlag::SemiTransparent)
				{
					tri.SetSemiTransparencyMode(color[0].g);
				}
			}
			m_triList.push_back(tri);

			triCount++;
			trisLinkedTex--;
			v[0] = v[1];
			v[1] = v[2];
			fileVertexCount += TRI_VERTEX_COUNT;
			flipTri = !flipTri;
		}
	}
}

std::streamoff Mesh::Load(std::ifstream &file)
{
	std::streamoff headerEnd = LoadHeader(file);
	LoadVertexCompression(file);
	LoadVcolorFlags(file);
	LoadUvIndexes(file);
	LoadTexinfos(file);
	if (m_isAnimated)
	{
		LoadAnimVertexIndex(file);
		LoadAnimVertexes(file);
	}
	else
	{
		LoadVertexes(file);
	}
	return headerEnd;
}

void Mesh::Export()
{
	std::ofstream obj(m_outputPath + ".obj", std::ios::out);
	obj << "mtllib " << m_name << ".mtl" << std::endl;
	for (Triangle &tri : m_triList)
	{
		obj << tri;
	}
	obj.close();
}