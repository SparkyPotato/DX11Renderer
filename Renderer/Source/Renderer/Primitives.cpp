#include <d3d11.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

#include "Primitives.h"

Microsoft::WRL::ComPtr<ID3D11Buffer> CreateVertexBuffer(ID3D11Device* device, unsigned int stride, unsigned int elementCount, BufferAccess access, void* data)
{
	UINT cpuAccess = 0;
	D3D11_USAGE usage = D3D11_USAGE_DEFAULT;
	if (access == BufferAccess::Dynamic)
	{
		cpuAccess = D3D11_CPU_ACCESS_WRITE;
		usage = D3D11_USAGE_DYNAMIC;
	}

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = stride * elementCount;
	desc.StructureByteStride = stride;
	desc.CPUAccessFlags = cpuAccess;
	desc.MiscFlags = NULL;
	desc.Usage = usage;

	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	HRESULT hr;
	if (data == nullptr)
		hr = device->CreateBuffer(&desc, NULL, &buffer);
	else
	{
		D3D11_SUBRESOURCE_DATA sr;
		sr.pSysMem = data;
		hr = device->CreateBuffer(&desc, &sr, &buffer);
	}

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create vertex buffer!", L"Object Error", MB_OK | MB_ICONERROR);
		return nullptr;
	}

	return buffer;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> CreateIndexBuffer(ID3D11Device* device, unsigned int indexCount, BufferAccess access, unsigned int* data)
{
	UINT cpuAccess = 0;
	D3D11_USAGE usage = D3D11_USAGE_DEFAULT;
	if (access == BufferAccess::Dynamic)
	{
		cpuAccess = D3D11_CPU_ACCESS_WRITE;
		usage = D3D11_USAGE_DYNAMIC;
	}

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.ByteWidth = sizeof(unsigned int) * indexCount;
	desc.StructureByteStride = sizeof(unsigned int);
	desc.CPUAccessFlags = cpuAccess;
	desc.MiscFlags = NULL;
	desc.Usage = usage;

	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	HRESULT hr;
	if (data == nullptr)
		hr = device->CreateBuffer(&desc, NULL, &buffer);
	else
	{
		D3D11_SUBRESOURCE_DATA sr;
		sr.pSysMem = data;
		hr = device->CreateBuffer(&desc, &sr, &buffer);
	}

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create index buffer!", L"Object Error", MB_OK | MB_ICONERROR);
		return nullptr;
	}

	return buffer;
}

Microsoft::WRL::ComPtr<ID3D11VertexShader> LoadVertexShader(ID3D11Device* device, std::wstring compiledPath)
{
	ID3DBlob* blob;
	HRESULT hr = D3DReadFileToBlob(compiledPath.c_str(), &blob);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to load vertex shader!", L"Object Error", MB_OK | MB_ICONERROR);
		return nullptr;
	}

	Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
	hr = device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, &shader);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create vertex shader!", L"Object Error", MB_OK | MB_ICONERROR);
		return nullptr;
	}
	blob->Release();

	return shader;
}

Microsoft::WRL::ComPtr<ID3D11VertexShader> CompileVertexShader(ID3D11Device* device, std::wstring sourcePath)
{
	ID3DBlob* codeBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(sourcePath.c_str(), NULL, NULL, "main", "vs_5_0", D3DCOMPILE_DEBUG, NULL, &codeBlob, &errorBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to compile vertex shader!", L"Object Error", MB_OK | MB_ICONERROR);
		if (errorBlob) OutputDebugStringA((char*) errorBlob->GetBufferPointer());
		return nullptr;
	}

	Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
	hr = device->CreateVertexShader(codeBlob->GetBufferPointer(), codeBlob->GetBufferSize(), NULL, &shader);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create vertex shader!", L"Object Error", MB_OK | MB_ICONERROR);
		return nullptr;
	}
	codeBlob->Release();
	if (errorBlob) errorBlob->Release();

	return shader;
}

Microsoft::WRL::ComPtr<ID3D11PixelShader> LoadPixelShader(ID3D11Device* device, std::wstring compiledPath)
{
	ID3DBlob* blob;
	HRESULT hr = D3DReadFileToBlob(compiledPath.c_str(), &blob);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to load pixel shader!", L"Object Error", MB_OK | MB_ICONERROR);
		return nullptr;
	}

	Microsoft::WRL::ComPtr<ID3D11PixelShader> shader;
	hr = device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, &shader);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create pixel shader!", L"Object Error", MB_OK | MB_ICONERROR);
		return nullptr;
	}
	blob->Release();

	return shader;
}

Microsoft::WRL::ComPtr<ID3D11PixelShader> CompilePixelShader(ID3D11Device* device, std::wstring sourcePath)
{
	ID3DBlob* codeBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(sourcePath.c_str(), NULL, NULL, "main", "ps_5_0", D3DCOMPILE_DEBUG, NULL, &codeBlob, &errorBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to compile pixel shader!", L"Object Error", MB_OK | MB_ICONERROR);
		if (errorBlob) OutputDebugStringA((char*) errorBlob->GetBufferPointer());
		return nullptr;
	}

	Microsoft::WRL::ComPtr<ID3D11PixelShader> shader;
	hr = device->CreatePixelShader(codeBlob->GetBufferPointer(), codeBlob->GetBufferSize(), NULL, &shader);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create pixel shader!", L"Object Error", MB_OK | MB_ICONERROR);
		return nullptr;
	}
	codeBlob->Release();
	if (errorBlob) errorBlob->Release();

	return shader;
}
