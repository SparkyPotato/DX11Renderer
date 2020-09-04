#include "Shader.h"
#include "GraphicsContext.h"
#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

VertexShader::VertexShader(std::wstring compiledPath)
{
	HRESULT hr = D3DReadFileToBlob(compiledPath.c_str(), &p_Blob);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to load vertex shader!", L"Object Error", MB_OK | MB_ICONERROR);
		__debugbreak();
	}

	hr = GraphicsContext::Device->CreateVertexShader(p_Blob->GetBufferPointer(), p_Blob->GetBufferSize(), NULL, &p_Shader);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create vertex shader!", L"Object Error", MB_OK | MB_ICONERROR);
		__debugbreak();
	}
}

VertexShader::~VertexShader()
{

}

VertexShader* VertexShader::Compile(std::wstring sourcePath, std::vector<ShaderDefine> defines /*= {}*/, std::string entrypoint /*= "main"*/)
{
	std::vector<D3D_SHADER_MACRO> d3dDefines;
	for (auto& define : defines)
	{
		d3dDefines.push_back({ define.define.c_str(), define.value.c_str() });
	}
	d3dDefines.push_back({ NULL, NULL });

	Microsoft::WRL::ComPtr<ID3DBlob> codeBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(sourcePath.c_str(), d3dDefines.data(), NULL, entrypoint.c_str(), "vs_5_0", D3DCOMPILE_DEBUG, NULL, &codeBlob, &errorBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to compile vertex shader!", L"Object Error", MB_OK | MB_ICONERROR);
		if (errorBlob) OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		__debugbreak();
		return nullptr;
	}

	Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
	hr = GraphicsContext::Device->CreateVertexShader(codeBlob->GetBufferPointer(), codeBlob->GetBufferSize(), NULL, &shader);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create vertex shader!", L"Object Error", MB_OK | MB_ICONERROR);
		__debugbreak();
		return nullptr;
	}
	if (errorBlob) errorBlob->Release();

	auto vShader = new VertexShader();
	vShader->p_Blob = codeBlob;
	vShader->p_Shader = shader;

	return vShader;
}

void VertexShader::Bind()
{
	GraphicsContext::BindVertexShader(this);
}

void VertexShader::Unbind()
{
	GraphicsContext::BindVertexShader(nullptr);
}

PixelShader::PixelShader(std::wstring compiledPath)
{
	HRESULT hr = D3DReadFileToBlob(compiledPath.c_str(), &p_Blob);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to load pixel shader!", L"Object Error", MB_OK | MB_ICONERROR);
		__debugbreak();
	}

	hr = GraphicsContext::Device->CreatePixelShader(p_Blob->GetBufferPointer(), p_Blob->GetBufferSize(), NULL, &p_Shader);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create pixel shader!", L"Object Error", MB_OK | MB_ICONERROR);
		__debugbreak();
	}
}

PixelShader::~PixelShader()
{

}

PixelShader* PixelShader::Compile(std::wstring sourcePath, std::vector<ShaderDefine> defines /*= {}*/, std::string entrypoint /*= "main"*/)
{
	std::vector<D3D_SHADER_MACRO> d3dDefines;
	for (auto& define : defines)
	{
		d3dDefines.push_back({ define.define.c_str(), define.value.c_str() });
	}
	d3dDefines.push_back({ NULL, NULL });

	Microsoft::WRL::ComPtr<ID3DBlob> codeBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(sourcePath.c_str(), d3dDefines.data(), NULL, entrypoint.c_str(), "ps_5_0", D3DCOMPILE_DEBUG, NULL, &codeBlob, &errorBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to compile pixel shader!", L"Object Error", MB_OK | MB_ICONERROR);
		if (errorBlob) OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		__debugbreak();
		return nullptr;
	}

	Microsoft::WRL::ComPtr<ID3D11PixelShader> shader;
	hr = GraphicsContext::Device->CreatePixelShader(codeBlob->GetBufferPointer(), codeBlob->GetBufferSize(), NULL, &shader);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create pixel shader!", L"Object Error", MB_OK | MB_ICONERROR);
		__debugbreak();
		return nullptr;
	}
	if (errorBlob) errorBlob->Release();

	auto pShader = new PixelShader();
	pShader->p_Blob = codeBlob;
	pShader->p_Shader = shader;

	return pShader;
}

void PixelShader::Bind()
{
	GraphicsContext::Context->PSSetShader(p_Shader.Get(), NULL, 0);
}

void PixelShader::Unbind()
{
	GraphicsContext::Context->PSSetShader(NULL, NULL, 0);
}
