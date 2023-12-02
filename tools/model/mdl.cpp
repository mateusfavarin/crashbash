#include "mdl.h"

MDL::MDL(fs::path mdlPath)
{
	m_path = mdlPath;
	m_file = std::ifstream(m_path, std::ios::in | std::ios::binary);
	m_fileBeg = m_file.tellg();
	m_vcolorDataPos = 0;

	m_name = m_path.stem().string();
	m_outputPath = m_name + "/";
	fs::create_directory(m_outputPath);

	ReadHeader();
	LoadMeshes();
	LoadAnims();
}

inline void MDL::FileSeekRelative(std::streamoff offset)
{
	m_file.seekg(m_fileBeg + offset, std::ios::beg);
}

inline void MDL::FileSeekAbsolute(std::streamoff position)
{
	m_file.seekg(position, std::ios::beg);
}

std::streamoff MDL::ReadHeader()
{
	FileSeekRelative();
	m_file.read((char *) &m_header, sizeof(m_header));
	m_vcolorDataPos = m_fileBeg + offsetof(MDLHeader, vcolorDataOffset) + m_header.vcolorDataOffset;
	return m_file.tellg();
}

std::streamoff MDL::LoadMeshes()
{
	FileSeekRelative(sizeof(m_header));
	for (unsigned i = 0; i < m_header.numMeshes; i++)
	{
		Mesh mesh = Mesh(m_outputPath, "mesh", i, m_file.tellg(), m_vcolorDataPos);
		std::streamoff nextHeader = mesh.Load(m_file);
		m_meshList.push_back(mesh);
		FileSeekAbsolute(nextHeader);
	}
	return m_file.tellg();
}

std::streamoff MDL::LoadAnims()
{
	FileSeekRelative(offsetof(MDLHeader, animHeaderOffset) + m_header.animHeaderOffset);
	for (unsigned i = 0; i < m_header.numAnimations; i++)
	{
		Anim anim = Anim(m_outputPath, i, m_file.tellg(), m_vcolorDataPos);
		std::streamoff nextHeader = anim.Load(m_file);
		m_animList.push_back(anim);
		FileSeekAbsolute(nextHeader);
	}
	return m_file.tellg();
}

void MDL::ToObj()
{
	for (Mesh &mesh : m_meshList)
	{
		mesh.ToObj();
	}
	for (Anim &anim : m_animList)
	{
		anim.ToObj();
	}
}