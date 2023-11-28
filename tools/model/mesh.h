#include <vector>
#include <string>
#include "primitives.h"

class Mesh
{
public:
	Mesh(const std::string & outputPath, unsigned index, std::ifstream * file);
	void ToObj();

private:
	unsigned m_index;
	std::string m_outputPath;
	std::ifstream * m_file;
	std::streamoff m_fileOffset;

	MeshHeader m_header;
	VertexHeader m_vertexHeader;
	std::vector<Triangle *> m_triList;
	std::vector<VertexEncode> m_vertexEncodeList;

	void LoadMesh();
};