#pragma once
#include <wrl.h>
#include <d3d11.h>

class VertexBuffer;
class ConstantBuffer;
class VertexShader;

class GraphicsContext
{
public:
	static void Init(HWND window);
	static void DeInit();

	static void BindVertexBuffer(const VertexBuffer* buffer);
	static void BindVertexShader(const VertexShader* shader);

	static void BindVSConstantBuffer(const ConstantBuffer* buffer, unsigned int slot);
	static void BindPSConstantBuffer(const ConstantBuffer* buffer, unsigned int slot);

	static Microsoft::WRL::ComPtr<ID3D11Device> Device;
	static Microsoft::WRL::ComPtr<ID3D11DeviceContext> Context;
	static Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain;

private:
	static void InputLayoutSetup();

	static const VertexBuffer* m_LastVertexBuffer;
	static const VertexBuffer* m_CurrentVertexBuffer;
	static const VertexShader* m_CurrentVertexShader;

	static ConstantBuffer* m_BoundVSConstantBuffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT];
	static ID3D11Buffer* m_VSConstantBuffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT];
	static ConstantBuffer* m_BoundPSConstantBuffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT];
	static ID3D11Buffer* m_PSConstantBuffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT];
};