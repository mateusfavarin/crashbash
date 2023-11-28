#pragma once
#include <ostream>
#include <stdint.h>

struct Point
{
	int16_t x;
	int16_t y;
	int16_t z;

	void InvertCoords()
	{
		y *= -1;
		z *= -1;
	}
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

	void InvertCoords()
	{
		y *= -1;
		z *= -1;
	}
};

struct TriVertex
{
	float x;
	float y;
	float z;
};

#define TRI_VERTEX_COUNT 3
#define TRI_SCALE 256.0
struct Triangle
{
	TriVertex v[TRI_VERTEX_COUNT];
	int vertexOffset;

	Triangle(Vertex * v, int vertexOffset, bool flipTri)
	{
		this->v[0].x = ((float)v[0].x) / TRI_SCALE;
		this->v[0].y = ((float)v[0].y) / TRI_SCALE;
		this->v[0].z = ((float)v[0].z) / TRI_SCALE;
		if (flipTri)
		{
			this->v[1].x = ((float)v[2].x) / TRI_SCALE;
			this->v[1].y = ((float)v[2].y) / TRI_SCALE;
			this->v[1].z = ((float)v[2].z) / TRI_SCALE;
			this->v[2].x = ((float)v[1].x) / TRI_SCALE;
			this->v[2].y = ((float)v[1].y) / TRI_SCALE;
			this->v[2].z = ((float)v[1].z) / TRI_SCALE;
		}
		else
		{
			this->v[1].x = ((float)v[1].x) / TRI_SCALE;
			this->v[1].y = ((float)v[1].y) / TRI_SCALE;
			this->v[1].z = ((float)v[1].z) / TRI_SCALE;
			this->v[2].x = ((float)v[2].x) / TRI_SCALE;
			this->v[2].y = ((float)v[2].y) / TRI_SCALE;
			this->v[2].z = ((float)v[2].z) / TRI_SCALE;
		}
		this->vertexOffset = vertexOffset;
	}
};

static std::ostream & operator<<(std::ostream &out, const Triangle &t)
{
	for (int i = 0; i < TRI_VERTEX_COUNT; i++)
	{
		out << "v " << t.v[i].x << " " << t.v[i].y << " " << t.v[i].z << std::endl;
	}
	return out << "f " << t.vertexOffset << " " << t.vertexOffset + 1 << " " << t.vertexOffset + 2 << std::endl;
}

struct VertexEncode
{
	uint8_t flag;
	uint8_t count;
};

struct MDLHeader
{
	uint8_t unk_0x0[0x28]; // 0x0
	uint32_t animVertexDataOffset; // 0x28
	uint8_t unk_0x2C[0x14];
	uint32_t numAnimations; // 0x40
	uint32_t animHeaderOffset; // 0x44
	uint8_t unk_0x48[0xC]; // 0x48
	uint32_t numMeshes; // 0x54
}; static_assert(sizeof(MDLHeader) == 0x58);

struct MeshHeader
{
	uint8_t unk0x0[0x10]; // 0x0
	uint32_t vertexListOffset; // 0x10
	uint32_t vertexEncodeOffset; // 0x14
	uint8_t unk0x18[0x1C]; // 0x18
}; static_assert(sizeof(MeshHeader) == 0x34);

struct VertexHeader
{
	BBox bbox; // 0x0
	Point unk0xC; // 0xC
	uint16_t unk0x12; // 0x12
}; static_assert(sizeof(VertexHeader) == 0x14);