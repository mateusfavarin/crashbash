#include "anim.h"

Anim::Anim(const std::string &outputPath, const std::string &name, unsigned index, std::streamoff fileBeg, std::streamoff vcolorDataPos, std::streamoff uvDataPos) :
	FileComponent(outputPath + "anim_" + std::to_string(index) + "_", index, fileBeg)
{
	m_name = name;
	m_vcolorDataPos = vcolorDataPos;
	m_uvDataPos = uvDataPos;
	m_numKeyframes = 0;
	m_header = {};
};

std::streamoff Anim::LoadHeader(std::ifstream &file)
{
	FileSeekRelative(file);
	file.read((char *) &m_header, sizeof(m_header));
	return file.tellg();
}

void Anim::LoadKeyframes(std::ifstream &file, Tex &tex)
{
	uint32_t animDataOffset;
	FileSeekAbsolute(file, m_header.dataStartPos);
	file.read((char *) &animDataOffset, sizeof(animDataOffset));

	std::streamoff meshBeg = m_fileBeg + offsetof(AnimHeader, meshHeaderOffset) + m_header.meshHeaderOffset;
	std::streamoff animDataPos = m_header.dataStartPos + animDataOffset;
	for (unsigned i = 0; i < m_header.numFrames; i++)
	{
		KeyframeHeader keyframeHeader;
		std::streamoff headerPos = file.tellg();
		file.read((char *) &keyframeHeader, sizeof(keyframeHeader));
		m_keyframeHeaderList.push_back(keyframeHeader);
		std::streamoff nextHeader = file.tellg();

		std::streamoff animIndexStartPos = headerPos + offsetof(KeyframeHeader, indexStartOffset) + keyframeHeader.indexStartOffset;
		std::streamoff animIndexEndPos = headerPos + offsetof(KeyframeHeader, indexEndOffset) + keyframeHeader.indexEndOffset;
		if (!m_keyframeMeshIndex.contains(animIndexStartPos))
		{
			m_keyframeMeshIndex[animIndexStartPos] = ++m_numKeyframes;
			Mesh mesh = Mesh(m_outputPath, "keyframe", m_name, m_numKeyframes, meshBeg, m_vcolorDataPos, m_uvDataPos, animIndexStartPos, animDataPos);
			mesh.Load(file);
			mesh.ConvertVertexesToTriangles(file, tex);
			m_meshList.push_back(mesh);
		}
		if ((keyframeHeader.interpolationRate != 0) && (!m_keyframeMeshIndex.contains(animIndexEndPos)))
		{
			m_keyframeMeshIndex[animIndexEndPos] = ++m_numKeyframes;
			Mesh mesh = Mesh(m_outputPath, "keyframe", m_name, m_numKeyframes, meshBeg, m_vcolorDataPos, m_uvDataPos, animIndexEndPos, animDataPos);
			mesh.Load(file);
			mesh.ConvertVertexesToTriangles(file, tex);
			m_meshList.push_back(mesh);
		}
		m_animItptList.emplace_back(AnimInterpolation(m_keyframeMeshIndex[animIndexStartPos], m_keyframeMeshIndex[animIndexEndPos], keyframeHeader.interpolationRate));

		FileSeekAbsolute(file, nextHeader);
	}
}

std::streamoff Anim::Load(std::ifstream &file)
{
	std::streamoff headerEnd = LoadHeader(file);
	return headerEnd;
}

void Anim::Export()
{
	std::ofstream keyframes(m_outputPath + "keyframes.txt", std::ios::out);
	keyframes << "Interpolation formula: v = [keyframeStart.v * (1 - rate)] + [keyframeEnd.v * rate]" << std::endl;
	keyframes << "keyframeStart | keyframeEnd | Interpolation Rate" << std::endl;
	for (AnimInterpolation &animIntp : m_animItptList)
	{
		keyframes << animIntp;
	}
	keyframes.close();
	for (Mesh &mesh : m_meshList)
	{
		mesh.Export();
	}
}