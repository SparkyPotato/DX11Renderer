#pragma once

class Scene;

class Renderer
{
public:
	Renderer(ID3D11Device* device, ID3D11DeviceContext* context, IDXGISwapChain* swapchain);
	~Renderer();

	void Render(float deltaTime);

	void Resize();

private:
	ID3D11Device* p_Device;
	ID3D11DeviceContext* p_Context;
	IDXGISwapChain* p_SwapChain;

	ID3D11RenderTargetView* p_RenderTarget = nullptr;
	ID3D11DepthStencilView* p_DepthStencil = nullptr;

	Scene* m_Scene = nullptr;
};
