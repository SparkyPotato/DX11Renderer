#include "Buffer.h"
#include "GraphicsContext.h"

VertexBuffer::VertexBuffer(const VertexLayout& layout, BufferAccess access, const void* data, unsigned int elementCount)
	: m_Size(elementCount), m_Layout(layout), m_Access(access)
{
	Create(data);
}

VertexBuffer::~VertexBuffer()
{
	GraphicsContext::BindVertexBuffer(nullptr);
}

void VertexBuffer::Bind() const
{
	GraphicsContext::BindVertexBuffer(this);
}

void VertexBuffer::Unbind() const
{
	GraphicsContext::BindVertexBuffer(nullptr);
}

void VertexBuffer::Set(const void* data, unsigned int elementCount)
{
	if (elementCount > m_Size || m_Access == BufferAccess::Static)
	{
		m_Size = elementCount;
		Create(data);
	}
	else
	{
		D3D11_MAPPED_SUBRESOURCE sr;
		GraphicsContext::Context->Map(p_Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, NULL, &sr);
		memcpy(sr.pData, data, elementCount * m_Layout.stride);
		GraphicsContext::Context->Unmap(p_Buffer.Get(), 0);
	}
}

void VertexBuffer::Create(const void* data)
{
	UINT cpuAccess = 0;
	D3D11_USAGE usage = D3D11_USAGE_DEFAULT;
	if (m_Access == BufferAccess::Dynamic)
	{
		cpuAccess = D3D11_CPU_ACCESS_WRITE;
		usage = D3D11_USAGE_DYNAMIC;
	}

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = m_Size * m_Layout.stride;
	desc.StructureByteStride = m_Layout.stride;
	desc.CPUAccessFlags = cpuAccess;
	desc.MiscFlags = NULL;
	desc.Usage = usage;

	HRESULT hr;
	if (data == nullptr)
		hr = GraphicsContext::Device->CreateBuffer(&desc, NULL, &p_Buffer);
	else
	{
		D3D11_SUBRESOURCE_DATA sr;
		sr.pSysMem = data;
		hr = GraphicsContext::Device->CreateBuffer(&desc, &sr, &p_Buffer);
	}

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create vertex buffer!", L"Object Error", MB_OK | MB_ICONERROR);
		__debugbreak();
	}
}

IndexBuffer::IndexBuffer(BufferAccess access, const unsigned int* indices, unsigned int indexCount)
	: m_Size(indexCount), m_Access(access)
{
	Create(indices);
}

void IndexBuffer::Bind() const
{
	GraphicsContext::Context->IASetIndexBuffer(p_Buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}

void IndexBuffer::Unbind() const
{
	GraphicsContext::Context->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
}

void IndexBuffer::Set(const unsigned int* indices, unsigned int indexCount)
{
	if (indexCount > m_Size || m_Access == BufferAccess::Static)
	{
		m_Size = indexCount;
		Create(indices);
	}
	else
	{
		D3D11_MAPPED_SUBRESOURCE sr;
		GraphicsContext::Context->Map(p_Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, NULL, &sr);
		memcpy(sr.pData, indices, indexCount * sizeof(unsigned int));
		GraphicsContext::Context->Unmap(p_Buffer.Get(), 0);
	}
}

void IndexBuffer::Create(const unsigned int* indices)
{
	UINT cpuAccess = 0;
	D3D11_USAGE usage = D3D11_USAGE_DEFAULT;
	if (m_Access == BufferAccess::Dynamic)
	{
		cpuAccess = D3D11_CPU_ACCESS_WRITE;
		usage = D3D11_USAGE_DYNAMIC;
	}

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.ByteWidth = sizeof(unsigned int) * m_Size;
	desc.StructureByteStride = sizeof(unsigned int);
	desc.CPUAccessFlags = cpuAccess;
	desc.MiscFlags = NULL;
	desc.Usage = usage;

	HRESULT hr;
	if (indices == nullptr)
		hr = GraphicsContext::Device->CreateBuffer(&desc, NULL, &p_Buffer);
	else
	{
		D3D11_SUBRESOURCE_DATA sr;
		sr.pSysMem = indices;
		hr = GraphicsContext::Device->CreateBuffer(&desc, &sr, &p_Buffer);
	}

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create index buffer!", L"Object Error", MB_OK | MB_ICONERROR);
		__debugbreak();
	}
}

ConstantBuffer::ConstantBuffer(const void* data, size_t size, ConstantBufferTarget target)
	: m_Size(size), m_Target(target)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = (UINT) size;
	desc.StructureByteStride = (UINT) size;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = NULL;
	desc.Usage = D3D11_USAGE_DYNAMIC;

	HRESULT hr;
	if (data == nullptr)
		hr = GraphicsContext::Device->CreateBuffer(&desc, NULL, &p_Buffer);
	else
	{
		D3D11_SUBRESOURCE_DATA sr;
		sr.pSysMem = data;
		hr = GraphicsContext::Device->CreateBuffer(&desc, &sr, &p_Buffer);
	}

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create constant buffer!", L"Object Error", MB_OK | MB_ICONERROR);
		__debugbreak();
	}
}

void ConstantBuffer::Bind(unsigned int slot)
{
	switch (m_Target)
	{
	case ConstantBufferTarget::VertexShader: GraphicsContext::BindVSConstantBuffer(this, slot); break;;
	case ConstantBufferTarget::PixelShader: GraphicsContext::BindPSConstantBuffer(this, slot); break;;
	}

	m_Slot = slot;
}

void ConstantBuffer::Unbind() const
{
	switch (m_Target)
	{
	case ConstantBufferTarget::VertexShader: GraphicsContext::BindVSConstantBuffer(nullptr, m_Slot); break;;
	case ConstantBufferTarget::PixelShader: GraphicsContext::BindPSConstantBuffer(nullptr, m_Slot); break;;
	}
}

void ConstantBuffer::Set(const void* data)
{
	D3D11_MAPPED_SUBRESOURCE sr;
	GraphicsContext::Context->Map(p_Buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, NULL, &sr);
	memcpy(sr.pData, data, m_Size);
	GraphicsContext::Context->Unmap(p_Buffer.Get(), 0);
}
