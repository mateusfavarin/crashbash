#include "mdl.h"

MDL::MDL(fs::path mdlPath)
{
	m_path = mdlPath;
	m_file = std::ifstream(m_path, std::ios::in | std::ios::binary);

	m_name = m_path.stem().string();
	m_outputPath = m_name + "/";
	fs::create_directory(m_outputPath);

	ReadHeader();
	LoadMeshes();
}

void MDL::ReadHeader()
{
	m_file.read((char *) &m_header, sizeof(m_header));
	m_file.seekg(0, std::ios::beg);
}

void MDL::LoadMeshes()
{
	m_file.seekg(sizeof(m_header), std::ios::beg);
	for (unsigned i = 0; i < m_header.numMeshes; i++)
	{
		Mesh * pMesh = new Mesh(m_outputPath, i, &m_file);
		m_meshList.push_back(pMesh);
	}
	m_file.seekg(0, std::ios::beg);
}

void MDL::ToObj()
{
	for (Mesh * pMesh : m_meshList)
	{
		pMesh->ToObj();
	}
}