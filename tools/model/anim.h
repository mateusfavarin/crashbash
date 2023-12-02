#pragma once
#include <unordered_map>
#include "primitives.h"
#include "filecomponent.h"
#include "mesh.h"

class Anim : public FileComponent
{
public:
	Anim(const std::string &outputPath, unsigned index, std::streamoff fileBeg, std::streamoff vcolorDataPos);
	std::streamoff Load(std::ifstream &file) override;
	void ToObj() override;

private:
	std::streamoff m_vcolorDataPos;

	AnimHeader m_header;
	unsigned m_numKeyframes;
	std::unordered_map<std::streamoff, unsigned> m_keyframeMeshIndex;
	std::vector<KeyframeHeader> m_keyframeHeaderList;
	std::vector<AnimInterpolation> m_animItptList;
	std::vector<Mesh> m_meshList;

	std::streamoff LoadHeader(std::ifstream &file);
	void LoadKeyframes(std::ifstream &file);
};