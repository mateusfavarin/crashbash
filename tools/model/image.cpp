#include "image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "third_party/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "third_party/stb_image_write.h"

Image::Image(const std::string &outputPath, unsigned index, std::streamoff fileBeg, std::streamoff clutPos, uint16_t numCluts) :
	FileComponent(outputPath + "tex_" + std::to_string(index) + ".png", index, fileBeg)
{
	m_clutPos = clutPos;
	m_lastClut = numCluts - 1;
	m_width = 0;
	m_height = 0;
	m_numColors = 0;
	m_bpp = 0;
	m_header = {};
}

std::streamoff Image::LoadClut(std::ifstream &file, unsigned index)
{
	Color1555 color;
	FileSeekAbsolute(file, m_clutPos);
	for (unsigned i = 0; i < index; i++)
	{
		file.read((char *) &m_numColors, sizeof(m_numColors));
		FileJump(file, m_numColors * sizeof(color));
	}
	file.read((char *) &m_numColors, sizeof(m_numColors));
	for (unsigned i = 0; i < m_numColors; i++)
	{
		file.read((char *) &color, sizeof(color));
		m_clut.push_back(color.Convert());
	}
	return file.tellg();
}

std::streamoff Image::Load(std::ifstream &file)
{
	FileSeekRelative(file);
	file.read((char *) &m_header, sizeof(m_header));
	unsigned clutIndex = m_header.paletteIndex >> 1;
	clutIndex = (clutIndex > m_lastClut) ? m_lastClut : clutIndex;
	LoadClut(file, clutIndex);

	uint16_t pixel;
	FileSeekRelative(file, sizeof(m_header));
	m_bpp = (m_numColors < 17) ? 4 : 8;
	unsigned widthMultiplier = (sizeof(pixel) * BITS_PER_BYTE) / m_bpp;
	m_width = widthMultiplier * m_header.width;
	m_height = m_header.height;
	unsigned pixelMask = (1 << m_bpp) - 1;
	for (unsigned i = 0; i < m_header.height * m_header.width; i++)
	{
		file.read((char *) &pixel, sizeof(pixel));
		for (unsigned j = 0; j < widthMultiplier; j++)
		{
			unsigned colorIndex = pixel & pixelMask;
			pixel = pixel >> m_bpp;
			m_image.push_back(m_clut[colorIndex]);
		}
	}
	return file.tellg();
}

const unsigned Image::GetWidth() const
{
	return m_width;
}

const unsigned Image::GetHeight() const
{
	return m_height;
}

void Image::Export()
{
	stbi_write_png(m_outputPath.c_str(), m_width, m_height, NUM_CHANNELS, &m_image[0], m_width * NUM_CHANNELS);
}
