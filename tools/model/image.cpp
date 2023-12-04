#include "image.h"

Image::Image(const std::string &outputPath, unsigned index, std::streamoff fileBeg) :
	FileComponent(outputPath + "img_" + std::to_string(index), index, fileBeg)
{
	m_width = 0;
	m_height = 0;
	m_numColors = 0;
	m_bpp = 0;
	m_header = {};
}

std::streamoff Image::LoadClut(std::ifstream &file)
{
	file.read((char *) &m_numColors, sizeof(m_numColors));
	Color1555 color;
	for (unsigned i = 0; i < m_numColors; i++)
	{
		file.read((char *) &color, sizeof(color));
		m_clut.push_back(color.Convert());
	}
	return file.tellg();
}

std::streamoff Image::Load(std::ifstream &file)
{
	file.read((char *) &m_header, sizeof(m_header));
	m_bpp = (m_header.unk0x4[8] & 1) ? 8 : 4;
	uint16_t pixel;
	m_width = ((sizeof(pixel) * BITS_PER_BYTE) / m_bpp) * m_header.width;
	m_height = m_header.height;
	std::cout << m_width << " " << m_height << std::endl;
	for (unsigned i = 0; i < m_header.height * m_header.width; i++)
	{
		file.read((char *) &pixel, sizeof(pixel));
	}
	return file.tellg();
}

const unsigned Image::getWidth() const
{
	return m_width;
}

const unsigned Image::getHeight() const
{
	return m_height;
}

void Image::Export()
{

}
