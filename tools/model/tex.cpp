#include "tex.h"

Tex::Tex(fs::path texPath) :
	FileManager(texPath, "tex")
{
	m_header = {};
	ReadHeader();
	LoadImages();
}

std::streamoff Tex::ReadHeader()
{
	FileSeekRelative();
	m_file.read((char *) &m_header, sizeof(m_header));
	return m_file.tellg();
}

std::streamoff Tex::LoadImages()
{
	for (unsigned i = 0; i < m_header.numCluts; i++)
	{
		Image image = Image(m_outputPath, i, m_file.tellg());
		image.LoadClut(m_file);
		m_imageList.push_back(image);
	}
	for (unsigned i = 0; i < m_header.numImages; i++)
	{
		m_imageList[i].Load(m_file);
	}
	return m_file.tellg();
}

void Tex::Export()
{
	for (Image &image : m_imageList)
	{
		//image.
	}
}

const Image& Tex::getImage(unsigned index) const
{
	return m_imageList[index];
}

const unsigned Tex::getImageListSize() const
{
	return m_imageList.size();
}

std::ostream &operator<<(std::ostream &out, const Tex &tex)
{
	for (unsigned i = 0; i < tex.getImageListSize(); i++)
	{
		out << "newmtl tex_" << i << std::endl << "map_Kd tex_" << i << ".bmp" << std::endl;
	}
	return out;
}
