#pragma once
#include <vector>
#include "filemanager.h"
#include "primitives.h"
#include "image.h"

class Tex : public FileManager
{
public:
	Tex(fs::path texPath, const std::string &mtlPath);
	void Export() override;
	const Image& GetImage(unsigned index) const;
	const unsigned GetImageListSize() const;

private:
	std::string m_mtlPath;
	TexHeader m_header;
	std::vector<Image> m_imageList;

	std::streamoff ReadHeader();
	std::streamoff LoadImages();
};

std::ostream &operator<<(std::ostream &out, const Tex &tex);
