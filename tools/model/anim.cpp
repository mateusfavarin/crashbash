#include "anim.h"

Anim::Anim(const std::string &outputPath, unsigned index, std::streamoff fileBeg) :
	FileComponent(outputPath + "anim_" + std::to_string(index) + "_", index, fileBeg)
{
	m_numKeyframes = 0;
	m_header = {};
};

std::streamoff Anim::LoadHeader(std::ifstream &file)
{
	FileSeekRelative(file);
	file.read((char *) &m_header, sizeof(m_header));
	return file.tellg();
}

void Anim::LoadKeyframes(std::ifstream &file)
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
			Mesh mesh = Mesh(m_outputPath, "keyframe", m_numKeyframes, meshBeg, animIndexStartPos, animDataPos);
			mesh.Load(file);
			m_meshList.push_back(mesh);
		}
		if ((keyframeHeader.interpolationRate != 0) && (!m_keyframeMeshIndex.contains(animIndexEndPos)))
		{
			m_keyframeMeshIndex[animIndexEndPos] = ++m_numKeyframes;
			Mesh mesh = Mesh(m_outputPath, "keyframe", m_numKeyframes, meshBeg, animIndexEndPos, animDataPos);
			mesh.Load(file);
			m_meshList.push_back(mesh);
		}
		m_animItptList.emplace_back(AnimInterpolation(m_keyframeMeshIndex[animIndexStartPos], m_keyframeMeshIndex[animIndexEndPos], keyframeHeader.interpolationRate));

		FileSeekAbsolute(file, nextHeader);
	}
}

std::streamoff Anim::Load(std::ifstream &file)
{
	std::streamoff headerEnd = LoadHeader(file);
	LoadKeyframes(file);
	return headerEnd;
}

void Anim::ToObj()
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
		mesh.ToObj();
	}
}