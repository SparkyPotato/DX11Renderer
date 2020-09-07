#pragma once
#include <string>
#include <vector>
#include <DirectXMath.h>

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

	struct 
	{
		float u, v;
	} texcoord;
};

class Object
{
public:
	Object(std::string name, std::string file);
	~Object() = default;

	std::string Name;

	const std::vector<Vertex>& GetVertices() { return m_Vertices; }
	const std::vector<unsigned int>& GetIndices() { return m_Indices; }

	DirectX::XMMATRIX GetWorldMatrix() { return m_World; }

private:
	std::vector<Vertex> m_Vertices;
	std::vector<unsigned int> m_Indices;
	DirectX::XMMATRIX m_World;
};
