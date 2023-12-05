#pragma once
#include <vector>
#include "primitives.h"
#include "filecomponent.h"

class Image : public FileComponent
{
public:
	Image(const std::string &outputPath, unsigned index, std::streamoff fileBeg, std::streamoff clutPos, uint16_t numCluts);
	std::streamoff Load(std::ifstream &file) override;
	void Export() override;
	const unsigned GetWidth() const;
	const unsigned GetHeight() const;

private:
	std::streamoff m_clutPos;
	uint16_t m_lastClut;

	ImageHeader m_header;
	unsigned m_width;
	unsigned m_height;
	unsigned m_bpp;
	uint32_t m_numColors;
	std::vector<Color> m_clut;
	std::vector<Color> m_image;

	std::streamoff LoadClut(std::ifstream &file, unsigned index);
};
