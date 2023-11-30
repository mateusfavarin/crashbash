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
	double x;
	double y;
	double z;
};

#define TRI_VERTEX_COUNT 3
#define TRI_SCALE 256.0
struct Triangle
{
	TriVertex v[TRI_VERTEX_COUNT];
	unsigned vertexOffset;

	Triangle(Vertex * v, unsigned vertexOffset, bool flipTri)
	{
		this->v[0].x = ((double)v[0].x) / TRI_SCALE;
		this->v[0].y = ((double)v[0].y) / TRI_SCALE;
		this->v[0].z = ((double)v[0].z) / TRI_SCALE;
		if (flipTri)
		{
			this->v[1].x = ((double)v[2].x) / TRI_SCALE;
			this->v[1].y = ((double)v[2].y) / TRI_SCALE;
			this->v[1].z = ((double)v[2].z) / TRI_SCALE;
			this->v[2].x = ((double)v[1].x) / TRI_SCALE;
			this->v[2].y = ((double)v[1].y) / TRI_SCALE;
			this->v[2].z = ((double)v[1].z) / TRI_SCALE;
		}
		else
		{
			this->v[1].x = ((double)v[1].x) / TRI_SCALE;
			this->v[1].y = ((double)v[1].y) / TRI_SCALE;
			this->v[1].z = ((double)v[1].z) / TRI_SCALE;
			this->v[2].x = ((double)v[2].x) / TRI_SCALE;
			this->v[2].y = ((double)v[2].y) / TRI_SCALE;
			this->v[2].z = ((double)v[2].z) / TRI_SCALE;
		}
		this->vertexOffset = vertexOffset;
	}
};

static std::ostream & operator<<(std::ostream &out, const Triangle &t)
{
	for (int i = 0; i < TRI_VERTEX_COUNT; i++)
	{
		out << "v " << t.v[i].x << " " << t.v[i].y << " " << t.v[i].z << " 0.7 0.7 0.7" << std::endl;
	}
	return out << "f " << t.vertexOffset << " " << t.vertexOffset + 1 << " " << t.vertexOffset + 2 << std::endl;
}

struct VertexCompression
{
	uint8_t flag;
	uint8_t count;
};

struct MDLHeader
{
	uint8_t unk0x0[0x28]; // 0x0
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
	uint8_t unk0x18[0x1C]; // 0x18
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

	AnimInterpolation(unsigned from, unsigned to, uint32_t interpolationRate)
	{
		this->from = from;
		this->to = to;
		this->rate = ((double) interpolationRate * 100) / 4096.0;
	}
};

static std::ostream &operator<<(std::ostream &out, const AnimInterpolation &animItpl)
{
	return out << animItpl.from << " | " << animItpl.to << " | " << animItpl.rate << "%" << std::endl;
}