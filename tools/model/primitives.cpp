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

Color1555 Color::Convert()
{
	Color1555 color;
	bool fixPitchBlack = false;
	switch (this->a)
	{
	case NO_TRANS: // no transparency
		color.a = 0;
		if ((color.r == 0) && (color.b == 0) && (color.r == 0))
		{
			fixPitchBlack = true;
		}
		break;
	case MAX_TRANS: // fully transparent
		color.a = 0;
		color.r = 0;
		color.g = 0;
		color.b = 0;
		return color;
	default: // semi transparent
		color.a = 1;
		break;
	}
	color.r = (((this->r * 249u) + 1014u) >> 11u) & 0x1Fu;
	color.g = (((this->g * 249u) + 1014u) >> 11u) & 0x1Fu;
	color.b = fixPitchBlack ? 1 : (((this->b * 249u) + 1014u) >> 11u) & 0x1Fu;
	return color;
}

Color Color1555::Convert()
{
	Color color;
	bool fixPitchBlack = false;
	if ((this->a == 0) && (this->r == 0) && (this->g == 0) && (this->b == 0))
	{
		color.a = MAX_TRANS;
	}
	else
	{
		if (this->a == 0)
		{
			color.a = NO_TRANS;
			if ((this->r == 0) && (this->g == 0) && (this->b == 1))
			{
				fixPitchBlack = true;
			}
		}
		else
		{
			color.a = SEMI_TRANS;
		}
	}
	color.r = (((this->r * 527u) + 23u) >> 6u) & 0xFFu;
	color.g = (((this->g * 527u) + 23u) >> 6u) & 0xFFu;
	color.b = fixPitchBlack ? 0 : (((this->b * 527u) + 23u) >> 6u) & 0xFFu;
	return color;
}

Triangle::Triangle(Vertex * v, unsigned vertexCount, unsigned uvCount, Color * vcolor, UV * uv, unsigned texWidth, unsigned texHeight, unsigned texIndex, bool isTextured, bool flipTri)
{
	texWidth--;
	texHeight--;
	for (unsigned i = 0; i < TRI_VERTEX_COUNT; i++)
	{
		this->v[i].x = ((double) v[i].x) / TRI_SCALE;
		this->v[i].y = ((double) v[i].y) / TRI_SCALE;
		this->v[i].z = ((double) v[i].z) / TRI_SCALE;
		this->v[i].r = ((double) vcolor[i].r) / RGB_MAX;
		this->v[i].g = ((double) vcolor[i].g) / RGB_MAX;
		this->v[i].b = ((double) vcolor[i].b) / RGB_MAX;
		this->v[i].u = ((double) uv[i].u) / ((double) texWidth);
		this->v[i].v = ((double) (texHeight - uv[i].v)) / ((double) texHeight);
	}
	if (flipTri)
	{
		TriVertex temp = this->v[1];
		this->v[1] = this->v[2];
		this->v[2] = temp;
	}
	this->vertexCount = vertexCount;
	this->uvCount = uvCount;
	this->texIndex = texIndex;
	this->isTextured = isTextured;
	this->semiTransparencyMode = 0;
}

void Triangle::SetSemiTransparencyMode(uint8_t g)
{
	this->semiTransparencyMode = (g >> 5) & 0b11;
}

std::ostream &operator<<(std::ostream &out, const Triangle &t)
{
	for (unsigned i = 0; i < TRI_VERTEX_COUNT; i++)
	{
		out << "v " << t.v[i].x << " " << t.v[i].y << " " << t.v[i].z << " ";
		out << t.v[i].r << " " << t.v[i].g << " " << t.v[i].b << std::endl;
		if (t.isTextured)
		{
			out << "vt " << t.v[i].u << " " << t.v[i].v << std::endl;
		}
	}
	if (t.isTextured)
	{
		out << "usemtl tex_" << t.texIndex << std::endl;
	}
	out << "f";
	for (unsigned i = 0; i < TRI_VERTEX_COUNT; i++)
	{
		out << " " << t.vertexCount + i;
		if (t.isTextured)
		{
			out << "/" << t.uvCount + i;
		}
	}
	return out << std::endl;
}

AnimInterpolation::AnimInterpolation(unsigned from, unsigned to, uint32_t interpolationRate)
{
	this->from = from;
	this->to = to;
	this->rate = ((double) interpolationRate * 100) / 4096.0;
}

std::ostream &operator<<(std::ostream &out, const AnimInterpolation &animItpl)
{
	return out << animItpl.from << " | " << animItpl.to << " | " << animItpl.rate << "%" << std::endl;
}