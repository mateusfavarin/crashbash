#include "primitives.h"

void Point::InvertCoords()
{
	this->y *= -1;
	this->z *= -1;
}

void Vertex::InvertCoords()
{
	this->y *= -1;
	this->z *= -1;
}

void Triangle::SetSemiTransparencyMode(uint8_t g)
{
	this->semiTransparencyMode = (g >> 5) & 0b11;
}

Triangle::Triangle(Vertex * v, unsigned vertexOffset, Color * vcolor, bool isTextured, bool flipTri)
{
	for (unsigned i = 0; i < TRI_VERTEX_COUNT; i++)
	{
		this->v[i].x = ((double)v[i].x) / TRI_SCALE;
		this->v[i].y = ((double)v[i].y) / TRI_SCALE;
		this->v[i].z = ((double)v[i].z) / TRI_SCALE;
		this->v[i].r = ((double)vcolor[i].r) / 255.0;
		this->v[i].g = ((double)vcolor[i].g) / 255.0;
		this->v[i].b = ((double)vcolor[i].b) / 255.0;
	}
	if (flipTri)
	{
		TriVertex temp = this->v[1];
		this->v[1] = this->v[2];
		this->v[2] = temp;
	}

	this->vertexOffset = vertexOffset;
	this->isTextured = isTextured;
	this->semiTransparencyMode = 0;
}

std::ostream &operator<<(std::ostream &out, const Triangle &t)
{
	for (int i = 0; i < TRI_VERTEX_COUNT; i++)
	{
		out << "v " << t.v[i].x << " " << t.v[i].y << " " << t.v[i].z << " ";
		out << t.v[i].r << " " << t.v[i].g << " " << t.v[i].b << std::endl;
	}
	return out << "f " << t.vertexOffset << " " << t.vertexOffset + 1 << " " << t.vertexOffset + 2 << std::endl;
}

AnimInterpolation::AnimInterpolation(unsigned from, unsigned to, uint32_t interpolationRate)
{
	this->from = from;
	this->to = to;
	this->rate = ((double)interpolationRate * 100) / 4096.0;
}

std::ostream &operator<<(std::ostream &out, const AnimInterpolation &animItpl)
{
	return out << animItpl.from << " | " << animItpl.to << " | " << animItpl.rate << "%" << std::endl;
}