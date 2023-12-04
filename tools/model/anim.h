#pragma once
#include <unordered_map>
#include "primitives.h"
#include "filecomponent.h"
#include "mesh.h"
#include "tex.h"

class Anim : public FileComponent
{
public:
	Anim(const std::string &outputPath, const std::string &name, unsigned index, std::streamoff fileBeg, std::streamoff vcolorDataPos, std::streamoff uvDataPos);
	std::streamoff Load(std::ifstream &file) override;
	void LoadKeyframes(std::ifstream &file, Tex &tex);
	void Export() override;

private:
	std::string m_name;
	std::streamoff m_vcolorDataPos;
	std::streamoff m_uvDataPos;

	AnimHeader m_header;
	unsigned m_numKeyframes;
	std::unordered_map<std::streamoff, unsigned> m_keyframeMeshIndex;
	std::vector<KeyframeHeader> m_keyframeHeaderList;
	std::vector<AnimInterpolation> m_animItptList;
	std::vector<Mesh> m_meshList;

	std::streamoff LoadHeader(std::ifstream &file);
};