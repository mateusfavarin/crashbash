#pragma once
#include <ostream>
#include <stdint.h>

#define BITS_PER_BYTE 8

enum VertexEncodeFlag : uint8_t
{
	Untextured = 0x1,
	FlipTriangle = 0x8,
};

enum VcolorFlag : uint16_t
{
	SemiTransparent = 0x8000,
	IDBits = 0x1FFF,
};

struct Point
{
	int16_t x;
	int16_t y;
	int16_t z;

	void InvertCoords();
};

struct BBox
{
	Point min;
	Point max;
};

struct Vertex
{
	int16_t x;
	int16_t y;
	int16_t z;
	uint16_t pad;

	void InvertCoords();
};

struct Color1555;
#define RGB_MAX 255.0
#define NO_TRANS 255
#define SEMI_TRANS 127
#define MAX_TRANS 0
#define NUM_CHANNELS 4
struct Color
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;

	Color1555 Convert();
};

struct Color1555
{
	uint16_t r : 5;
	uint16_t g : 5;
	uint16_t b : 5;
	uint16_t a : 1;

	Color Convert();
};

struct UV
{
	uint8_t u;
	uint8_t v;
};

struct Texinfo
{
	uint16_t index    : 9;
	uint16_t triCount : 6;
	uint16_t unkFlag  : 1;
};

struct TriVertex
{
	double x;
	double y;
	double z;
	double r;
	double g;
	double b;
	double u;
	double v;
};

#define TRI_VERTEX_COUNT 3
#define TRI_SCALE 256.0
struct Triangle
{
	TriVertex v[TRI_VERTEX_COUNT];
	unsigned vertexCount;
	unsigned uvCount;
	unsigned texIndex;
	bool isTextured;
	uint16_t semiTransparencyMode;

	Triangle(Vertex * v, unsigned vertexCount, unsigned uvCount, Color * vcolor, UV * uv, const unsigned texWidth, const unsigned texHeight, unsigned texIndex, bool isTextured, bool flipTri);
	void SetSemiTransparencyMode(uint8_t g);
};

struct VertexCompression
{
	uint8_t flag;
	uint8_t count;
};

struct MDLHeader
{
	uint8_t unk0x0[0x20]; // 0x0
	uint32_t vcolorDataOffset; // 0x20
	uint32_t uvDataOffset; // 0x24
	uint32_t unk0x28; // 0x28
	uint8_t unk0x2C[0x14]; // 0x2C
	uint32_t numAnimations; // 0x40
	uint32_t animHeaderOffset; // 0x44
	uint8_t unk0x48[0xC]; // 0x48
	uint32_t numMeshes; // 0x54
}; static_assert(sizeof(MDLHeader) == 0x58);

struct MeshHeader
{
	uint8_t unk_0x0[0x10]; // 0x0
	uint32_t vertexListOffset; // 0x10
	uint32_t vertexCompressionOffset; // 0x14
	uint32_t uvIndexOffset; // 0x18
	uint32_t texinfoOffset; // 0x1C
	uint32_t vcolorFlagsOffset; // 0x20
	uint8_t unk0x24[0x10]; // 0x24
}; static_assert(sizeof(MeshHeader) == 0x34);

struct VertexHeader
{
	BBox bbox; // 0x0
	Point unk0xC; // 0xC
	uint16_t unk0x12; // 0x12
}; static_assert(sizeof(VertexHeader) == 0x14);

struct AnimHeader
{
	uint32_t dataStartPos; // 0x0
	uint32_t dataEndPos; // 0x4
	uint32_t numFrames; // 0x8
	int32_t meshHeaderOffset; // 0xC
	uint32_t unk0x10; // 0x10
	uint32_t ptrInGame_dataStart; // 0x14
}; static_assert(sizeof(AnimHeader) == 0x18);

struct KeyframeHeader
{
	uint32_t indexStartOffset; // 0x0
	uint32_t indexEndOffset; // 0x4
	uint32_t interpolationRate; // 0x8
	uint32_t unk0xC; // 0xC
}; static_assert(sizeof(KeyframeHeader) == 0x10);

struct AnimInterpolation
{
	unsigned from;
	unsigned to;
	double rate;

	AnimInterpolation(unsigned from, unsigned to, uint32_t interpolationRate);
};

struct TexHeader
{
	uint32_t magic; // 0x0
	uint32_t size; // 0x4
	uint16_t numImages; // 0x8
	uint16_t numCluts; // 0xA
	uint32_t imageOffset; // 0xC
	uint32_t clutOffset; // 0x10
	uint32_t unkOffset0x14; // 0x14
	uint32_t unkOffset0x18; // 0x18
	uint32_t unk0x1C; // 0x1C
}; static_assert(sizeof(TexHeader) == 0x20);

struct ImageHeader
{
	uint16_t width; // 0x0
	uint16_t height; // 0x2
	uint8_t unk0x4[8]; // 0x4
	uint16_t paletteIndex; // 0xC
	uint8_t unk0xE[6]; // 0xE
}; static_assert(sizeof(ImageHeader) == 0x14);

std::ostream &operator<<(std::ostream &out, const Triangle &t);
std::ostream &operator<<(std::ostream &out, const AnimInterpolation &animItpl);