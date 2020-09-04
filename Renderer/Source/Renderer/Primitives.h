#pragma once
#include <wrl.h>
#include <string>

enum class BufferAccess
{
	Static, Dynamic
};

Microsoft::WRL::ComPtr<ID3D11Buffer> CreateVertexBuffer
(
	ID3D11Device* device, 
	unsigned int stride, 
	unsigned int elementCount, 
	BufferAccess access, 
	void* data
);

Microsoft::WRL::ComPtr<ID3D11Buffer> CreateIndexBuffer
(
	ID3D11Device* device,
	unsigned int indexCount,
	BufferAccess access,
	unsigned int* data
);

Microsoft::WRL::ComPtr<ID3D11VertexShader> LoadVertexShader
(
	ID3D11Device* device,
	std::wstring compiledPath
);

Microsoft::WRL::ComPtr<ID3D11VertexShader> CompileVertexShader
(
	ID3D11Device* device,
	std::wstring sourcePath
);

Microsoft::WRL::ComPtr<ID3D11PixelShader> LoadPixelShader
(
	ID3D11Device* device,
	std::wstring compiledPath
);

Microsoft::WRL::ComPtr<ID3D11PixelShader> CompilePixelShader
(
	ID3D11Device* device,
	std::wstring sourcePath
);
