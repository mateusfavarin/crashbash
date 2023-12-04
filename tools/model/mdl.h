#pragma once
#include <vector>
#include "filemanager.h"
#include "tex.h"
#include "mesh.h"
#include "anim.h"

class MDL : public FileManager
{
public:
	MDL(fs::path mdlPath, fs::path texPath);
	void Export() override;

private:
	std::streamoff m_vcolorDataPos;
	std::streamoff m_uvDataPos;

	Tex m_tex;
	MDLHeader m_header;
	std::vector<Mesh> m_meshList;
	std::vector<Anim> m_animList;

	std::streamoff ReadHeader();
	std::streamoff LoadMeshes();
	std::streamoff LoadAnims();
};