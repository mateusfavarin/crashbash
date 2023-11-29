#include <vector>
#include <fstream>
#include "primitives.h"
#include "filecomponent.h"

class Mesh : public FileComponent
{
public:
	Mesh(const std::string &outputPath, unsigned index, std::streamoff fileBeg);
	std::streamoff Load(std::ifstream &file) override;
	void ToObj() override;

private:
	MeshHeader m_header;
	VertexHeader m_vertexHeader;
	std::vector<Triangle> m_triList;
	std::vector<VertexEncode> m_vertexEncodeList;

	std::streamoff LoadHeader(std::ifstream &file);
	std::streamoff LoadVertexes(std::ifstream &file);
	std::streamoff LoadVertexEncode(std::ifstream &file);
};