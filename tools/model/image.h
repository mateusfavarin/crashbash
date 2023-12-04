#pragma once
#include <vector>
#include "primitives.h"
#include "filecomponent.h"

class Image : FileComponent
{
public:
	Image(const std::string &outputPath, unsigned index, std::streamoff fileBeg);
	std::streamoff LoadClut(std::ifstream &file);
	std::streamoff Load(std::ifstream &file) override;
	void Export() override;
	const unsigned getWidth() const;
	const unsigned getHeight() const;

private:
	ImageHeader m_header;
	unsigned m_width;
	unsigned m_height;
	unsigned m_bpp;
	uint32_t m_numColors;
	std::vector<Color> m_clut;
};
