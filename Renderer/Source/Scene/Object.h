#pragma once
#include <string>
#include <vector>
#include <DirectXMath.h>

#include "Primitives/Buffer.h"

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

struct Material
{
	float color[4] = { 1.f, 1.f, 1.f, 1.f };
	float ambient = 1.f;
	float specular = 1.f;
	float diffuse = 1.f;
	float shininess = 4.f;
};

class Object
{
public:
	Object(std::string name, std::string file);
	Object(const Object& other);
	Object(Object&& other) noexcept;
	~Object() noexcept;

	Object& operator =(const Object& other)
	{
		Name = other.Name;
		return *this;
	}

	bool operator ==(Object other)
	{
		return other.Name == Name;
	}

	std::string Name;

	Material& GetMaterial() { return m_Material; }
	const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
	const std::vector<unsigned int>& GetIndices() const { return m_Indices; }

	const VertexBuffer* GetVertexBuffer() const { return m_VertexBuffer; }
	const IndexBuffer* GetIndexBuffer() const { return m_IndexBuffer; }

	const DirectX::XMMATRIX& GetWorldMatrix() const { return m_World; }

private:
	std::vector<Vertex> m_Vertices;
	std::vector<unsigned int> m_Indices;
	Material m_Material;
	DirectX::XMMATRIX m_World;

	VertexBuffer* m_VertexBuffer = nullptr;
	IndexBuffer* m_IndexBuffer = nullptr;

	inline static VertexLayout m_VertexLayout =
	{
		{ "POSITION", ElementDataType::float3 },
		{ "NORMAL", ElementDataType::float3 },
		{ "COLOR", ElementDataType::float4 },
		{ "TEXCOORD", ElementDataType::float2 }
	};
};
