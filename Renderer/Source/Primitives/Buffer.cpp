#include "Buffer.h"
#include "GraphicsContext.h"

VertexBuffer::VertexBuffer(const VertexLayout& layout, BufferAccess access, void* data, unsigned int elementCount)
	: m_Size(elementCount), m_Layout(layout), m_Access(access)
{
	Create(data);
}

VertexBuffer::~VertexBuffer()
{
	
}

void VertexBuffer::Bind()
{
	GraphicsContext::BindVertexBuffer(this);
}

void VertexBuffer::Unbind()
{
	GraphicsContext::BindVertexBuffer(nullptr);
}

void VertexBuffer::Set(void* data, unsigned int elementCount)
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

void VertexBuffer::Create(void* data)
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

IndexBuffer::IndexBuffer(BufferAccess access, unsigned int* indices, unsigned int indexCount)
	: m_Size(indexCount), m_Access(access)
{
	Create(indices);
}

IndexBuffer::~IndexBuffer()
{

}

void IndexBuffer::Bind()
{
	GraphicsContext::Context->IASetIndexBuffer(p_Buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}

void IndexBuffer::Unbind()
{
	GraphicsContext::Context->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
}

void IndexBuffer::Set(unsigned int* indices, unsigned int indexCount)
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

void IndexBuffer::Create(unsigned int* indices)
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

	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	HRESULT hr;
	if (indices == nullptr)
		hr = GraphicsContext::Device->CreateBuffer(&desc, NULL, &buffer);
	else
	{
		D3D11_SUBRESOURCE_DATA sr;
		sr.pSysMem = indices;
		hr = GraphicsContext::Device->CreateBuffer(&desc, &sr, &buffer);
	}

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create index buffer!", L"Object Error", MB_OK | MB_ICONERROR);
		__debugbreak();
	}
}
