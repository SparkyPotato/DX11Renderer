#include <exception>
#include <d3d11.h>

#include "Renderer.h"

Renderer::Renderer(ID3D11Device* device, ID3D11DeviceContext* context, IDXGISwapChain* swapchain)
	: p_Device(device), p_Context(context), p_SwapChain(swapchain)
{
	ID3D11Texture2D* backBuffer;
	HRESULT hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &backBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to get window back buffer!", L"Startup Error", MB_OK | MB_ICONERROR);
		throw std::exception("Render Target creation failed");
	}

	hr = p_Device->CreateRenderTargetView(backBuffer, NULL, &p_RenderTarget);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create Render Target View!", L"Startup Error", MB_OK | MB_ICONERROR);
		throw std::exception("Render Target creation failed");
	}
	backBuffer->Release();
}

Renderer::~Renderer()
{
	if (p_RenderTarget) p_RenderTarget->Release();
}

void Renderer::Render(float deltaTime)
{
	float color[] = { 0.11f, 0.18f, 0.96f, 1.f };
	p_Context->ClearRenderTargetView(p_RenderTarget, color);
}

void Renderer::Resize()
{
	if (p_RenderTarget) p_RenderTarget->Release();

	HRESULT hr = p_SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to resize swap chain!", L"Runtime Error", MB_OK | MB_ICONERROR);
	}

	ID3D11Texture2D* backBuffer;
	hr = p_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to get window back buffer!", L"Runtime Error", MB_OK | MB_ICONERROR);
		throw std::exception("Render Target creation failed");
	}

	hr = p_Device->CreateRenderTargetView(backBuffer, NULL, &p_RenderTarget);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create Render Target View!", L"Runtime Error", MB_OK | MB_ICONERROR);
		throw std::exception("Render Target creation failed");
	}
	backBuffer->Release();

	p_Context->Flush();
}
