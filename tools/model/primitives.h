#pragma once
#include <ostream>
#include <stdint.h>

enum VertexEncodeFlag : uint8_t
{
	Untextured = 0x1,
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

struct Color
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t pad;
};

struct TriVertex
{
	double x;
	double y;
	double z;
	double r;
	double g;
	double b;
};

#define TRI_VERTEX_COUNT 3
#define TRI_SCALE 256.0
struct Triangle
{
	TriVertex v[TRI_VERTEX_COUNT];
	unsigned vertexOffset;
	bool isTextured;
	uint16_t semiTransparencyMode;

	Triangle(Vertex * v, unsigned vertexOffset, Color * vcolor, bool isTextured, bool flipTri);
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
	uint32_t unk0x24; // 0x24
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
	uint8_t unk0x18[0x8]; // 0x18
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

std::ostream &operator<<(std::ostream &out, const Triangle &t);
std::ostream &operator<<(std::ostream &out, const AnimInterpolation &animItpl);