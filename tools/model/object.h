#pragma once
#include "primitives.h"
#include "filecomponent.h"
#include "mesh.h"
#include "tex.h"

class Object : public FileComponent
{
public:
	Object(const std::string &outputPath, const std::string &name, unsigned index, std::streamoff fileBeg, std::streamoff vcolorDataPos, std::streamoff uvDataPos);
	std::streamoff Load(std::ifstream &file) override;
	bool IsValid();
	void ConvertVertexesToTriangles(std::ifstream &file, Tex &tex);
	void Export() override;

private:
	std::string m_name;
	std::streamoff m_vcolorDataPos;
	std::streamoff m_uvDataPos;
	bool m_isValidData;

	ObjectHeader m_header;
	Mesh m_mesh;
};