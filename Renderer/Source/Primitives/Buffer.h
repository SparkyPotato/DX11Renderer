#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <vector>
#include <string>
#include <initializer_list>

enum class BufferAccess
{
	Static, Dynamic
};

enum class ElementDataType
{
	float1, float2, float3, float4
};

struct VertexElement
{
	std::string semantic;
	ElementDataType type;

	bool operator==(const VertexElement& other) const
	{
		return semantic == other.semantic && type == other.type;
	}

	bool operator!=(const VertexElement& other) const
	{
		return semantic != other.semantic || type != other.type;
	}

	unsigned int GetSize() const
	{
		switch (type)
		{
		case ElementDataType::float1: return (unsigned int) sizeof(float);
		case ElementDataType::float2: return (unsigned int) sizeof(float) * 2;
		case ElementDataType::float3: return (unsigned int) sizeof(float) * 3;
		case ElementDataType::float4: return (unsigned int) sizeof(float) * 4;
		}

		return 0;
	}
};

struct VertexLayout
{
	VertexLayout(std::initializer_list<VertexElement> list)
		: elements(list)
	{
		for (auto& element : elements)
		{
			stride += element.GetSize();
		}
	}

	bool operator==(const VertexLayout& other) const
	{
		if (elements.size() != other.elements.size())
			return false;

		for (int i = 0; i < elements.size(); i++)
		{
			if (elements[i] != other.elements[i]) return false;
		}

		return true;
	}

	bool operator!=(const VertexLayout& other) const
	{
		if (elements.size() != other.elements.size())
			return true;

		for (int i = 0; i < elements.size(); i++)
		{
			if (elements[i] != other.elements[i]) return true;
		}

		return false;
	}

	std::vector<VertexElement> elements;
	unsigned int stride = 0;
};

class VertexBuffer
{
public:
	VertexBuffer(const VertexLayout& layout, BufferAccess access, void* data, unsigned int elementCount);
	~VertexBuffer() = default;

	void Bind();
	void Unbind();

	void Set(void* data, unsigned int elementCount);

	const VertexLayout& GetLayout() { return m_Layout; }
	ID3D11Buffer* GetBuffer() { return p_Buffer.Get(); }

private:
	void Create(void* data);

	Microsoft::WRL::ComPtr<ID3D11Buffer> p_Buffer = nullptr;

	unsigned int m_Size;
	VertexLayout m_Layout;
	BufferAccess m_Access;
};

class IndexBuffer
{
public:
	IndexBuffer(BufferAccess access, unsigned int* indices, unsigned int indexCount);
	~IndexBuffer() = default;

	void Bind();
	void Unbind();

	void Set(unsigned int* indices, unsigned int indexCount);

	ID3D11Buffer* GetBuffer() { return p_Buffer.Get(); }

private:
	void Create(unsigned int* indices);

	Microsoft::WRL::ComPtr<ID3D11Buffer> p_Buffer = nullptr;
	
	unsigned int m_Size;
	BufferAccess m_Access;
};

enum class ConstantBufferTarget
{
	VertexShader, PixelShader
};

class ConstantBuffer
{
public:
	ConstantBuffer(void* data, size_t size, ConstantBufferTarget target);
	~ConstantBuffer() = default;

	void Bind(unsigned int slot = 0);
	void Unbind();

	void Set(void* data);

	ID3D11Buffer* GetBuffer() { return p_Buffer.Get(); }
	void SetUnboundSlot() { m_Slot = -1; }

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> p_Buffer = nullptr;

	size_t m_Size;
	unsigned int m_Slot = -1;
	ConstantBufferTarget m_Target;
};
