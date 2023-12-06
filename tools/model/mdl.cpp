#include "mdl.h"

MDL::MDL(fs::path mdlPath, fs::path texPath) :
	FileManager(mdlPath, "mdl"), m_tex(texPath, m_outputPath + m_name + ".mtl")
{
	m_vcolorDataPos = 0;
	m_uvDataPos = 0;
	m_numObjects = 0;

	ReadHeader();
	LoadMeshes();
	LoadAnims();
	LoadObjects();
}

std::streamoff MDL::ReadHeader()
{
	FileSeekRelative();
	m_file.read((char *) &m_header, sizeof(m_header));
	m_vcolorDataPos = m_fileBeg + offsetof(MDLHeader, vcolorDataOffset) + m_header.vcolorDataOffset;
	m_uvDataPos = m_fileBeg + offsetof(MDLHeader, uvDataOffset) + m_header.uvDataOffset;
	return m_file.tellg();
}

std::streamoff MDL::LoadMeshes()
{
	FileSeekRelative(sizeof(m_header));
	for (unsigned i = 0; i < m_header.numMeshes; i++)
	{
		Mesh mesh = Mesh(m_outputPath, "mesh", m_name, i, m_file.tellg(), m_vcolorDataPos, m_uvDataPos);
		std::streamoff nextHeader = mesh.Load(m_file);
		mesh.ConvertVertexesToTriangles(m_file, m_tex);
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
		Anim anim = Anim(m_outputPath, m_name, i, m_file.tellg(), m_vcolorDataPos, m_uvDataPos);
		std::streamoff nextHeader = anim.Load(m_file);
		anim.LoadKeyframes(m_file, m_tex);
		m_animList.push_back(anim);
		FileSeekAbsolute(nextHeader);
	}
	return m_file.tellg();
}

std::streamoff MDL::LoadObjects()
{
	FileSeekRelative(offsetof(MDLHeader, objectHeaderOffset) + m_header.objectHeaderOffset);
	m_file.read((char *) &m_numObjects, sizeof(m_numObjects));
	for (unsigned i = 0; i < m_numObjects; i++)
	{
		Object object = Object(m_outputPath, m_name, i, m_file.tellg(), m_vcolorDataPos, m_uvDataPos);
		std::streamoff nextHeader = object.Load(m_file);
		object.ConvertVertexesToTriangles(m_file, m_tex);
		m_objectList.push_back(object);
		FileSeekAbsolute(nextHeader);
	}
	return m_file.tellg();
}

void MDL::Export()
{
	m_tex.Export();
	for (Mesh &mesh : m_meshList)
	{
		mesh.Export();
	}
	for (Anim &anim : m_animList)
	{
		anim.Export();
	}
	for (Object &object : m_objectList)
	{
		object.Export();
	}
}