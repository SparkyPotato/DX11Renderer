#pragma once
#include <wrl.h>
#include <d3d11.h>

class VertexBuffer;
class VertexShader;

class GraphicsContext
{
public:
	static void Init(HWND window);

	static void BindVertexBuffer(VertexBuffer* buffer);
	static void BindVertexShader(VertexShader* shader);

	static Microsoft::WRL::ComPtr<ID3D11Device> Device;
	static Microsoft::WRL::ComPtr<ID3D11DeviceContext> Context;
	static Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain;

private:
	static void InputLayoutSetup();

	static VertexBuffer* m_LastVertexBuffer;
	static VertexBuffer* m_CurrentVertexBuffer;
	static VertexShader* m_CurrentVertexShader;
};