#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <string>
#include <vector>

struct ShaderDefine
{
	std::string define;
	std::string value;
};

class VertexShader
{
public:
	explicit VertexShader() {}
	VertexShader(std::wstring compiledPath);
	~VertexShader();

	static VertexShader* Compile(std::wstring sourcePath, std::vector<ShaderDefine> defines = {}, std::string entrypoint = "main");

	void Bind();
	void Unbind();

	ID3D11VertexShader* GetShader() { return p_Shader.Get(); }
	ID3DBlob* GetBlob() { return p_Blob.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> p_Shader;
	Microsoft::WRL::ComPtr<ID3DBlob> p_Blob;
};

class PixelShader
{
public:
	explicit PixelShader() {}
	PixelShader(std::wstring compiledPath);
	~PixelShader();

	static PixelShader* Compile(std::wstring sourcePath, std::vector<ShaderDefine> defines = {}, std::string entrypoint = "main");

	void Bind();
	void Unbind();

private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> p_Shader;
	Microsoft::WRL::ComPtr<ID3DBlob> p_Blob;
};
