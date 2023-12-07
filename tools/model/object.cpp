#include "object.h"

Object::Object(const std::string &outputPath, const std::string &name, unsigned index, std::streamoff fileBeg, std::streamoff vcolorDataPos, std::streamoff uvDataPos) :
	FileComponent(outputPath, index, fileBeg)
{
	m_isValidData = true;
	m_name = name;
	m_vcolorDataPos = vcolorDataPos;
	m_uvDataPos = uvDataPos;
	m_header = {};
}

std::streamoff Object::Load(std::ifstream &file)
{
	FileSeekRelative(file);
	file.read((char *) &m_header, sizeof(m_header));
	if (m_header.dataOffset >= 0)
	{
		m_isValidData = false;
		return std::streamoff();
	}
	std::streamoff headerEnd = file.tellg();

	uint32_t dataPos; // might always point to zero
	FileSeekRelative(file, offsetof(ObjectHeader, dataOffset) + m_header.dataOffset);
	file.read((char *) &dataPos, sizeof(dataPos));

	std::streamoff meshPos = dataPos + m_header.meshOffset;
	m_mesh = Mesh(m_outputPath, "object", m_name, m_index, meshPos, m_vcolorDataPos, m_uvDataPos);
	m_mesh.Load(file);

	return headerEnd;
}

bool Object::IsValid()
{
	return m_isValidData;
}

void Object::ConvertVertexesToTriangles(std::ifstream &file, Tex &tex)
{
	m_mesh.ConvertVertexesToTriangles(file, tex);
}

void Object::Export()
{
	m_mesh.Export();
}
