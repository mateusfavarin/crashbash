#include "mdl.h"

MDL::MDL(fs::path mdlPath)
{
	m_path = mdlPath;
	m_file = std::ifstream(m_path, std::ios::in | std::ios::binary);
	m_fileBeg = m_file.tellg();

	m_name = m_path.stem().string();
	m_outputPath = m_name + "/";
	fs::create_directory(m_outputPath);

	ReadHeader();
	LoadMeshes();
}

inline void MDL::FileSeekBeg(std::streamoff offset = 0)
{
	m_file.seekg(m_fileBeg + offset, std::ios::beg);
}

void MDL::ReadHeader()
{
	FileSeekBeg();
	m_file.read((char *) &m_header, sizeof(m_header));
}

void MDL::LoadMeshes()
{
	FileSeekBeg(sizeof(m_header));
	for (unsigned i = 0; i < m_header.numMeshes; i++)
	{
		Mesh mesh = Mesh(m_outputPath, i, m_file.tellg());
		std::streamoff nextHeader = mesh.Load(m_file);
		m_meshList.emplace_back(mesh);
		FileSeekBeg(nextHeader);
	}
}

void MDL::LoadAnims()
{
	FileSeekBeg(offsetof(MDLHeader, animHeaderOffset) + m_header.animHeaderOffset);
	for (unsigned i = 0; i < m_header.numAnimations; i++)
	{
		m_animList.emplace_back(Anim());
	}
}

void MDL::ToObj()
{
	for (Mesh &mesh : m_meshList)
	{
		mesh.ToObj();
	}
}