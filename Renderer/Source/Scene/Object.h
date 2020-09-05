#pragma once
#include <string>
#include <vector>

struct Vertex
{
	struct
	{
		float x, y, z;
	} position, normal;

	struct
	{
		float r, g, b, a;
	} color;
};

class Object
{
public:
	Object(std::string file);
	~Object() = default;

private:
	std::vector<Vertex> m_Vertices;
	std::vector<unsigned int> m_Indices;
};
