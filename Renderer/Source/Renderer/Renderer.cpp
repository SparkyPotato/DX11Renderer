#include <exception>
#include <d3d11.h>

#include "Renderer.h"
#include "Scene/Scene.h"
#include "Primitives/GraphicsContext.h"
#include "Primitives/Buffer.h"

Renderer::Renderer()
{
	ID3D11Texture2D* backBuffer;
	HRESULT hr = GraphicsContext::SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &backBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to get window back buffer!", L"Startup Error", MB_OK | MB_ICONERROR);
		this->~Renderer();
		throw std::exception("Render Target creation failed");
	}

	hr = GraphicsContext::Device->CreateRenderTargetView(backBuffer, NULL, &p_RenderTarget);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create Render Target View!", L"Startup Error", MB_OK | MB_ICONERROR);
		this->~Renderer();
		throw std::exception("Render Target creation failed");
	}

	D3D11_TEXTURE2D_DESC bbDesc;
	backBuffer->GetDesc(&bbDesc);
	backBuffer->Release();

	ID3D11Texture2D* depthStencilTexture;
	D3D11_TEXTURE2D_DESC dstDesc;
	ZeroMemory(&dstDesc, sizeof(D3D11_TEXTURE2D_DESC));
	dstDesc.ArraySize = 1;
	dstDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dstDesc.CPUAccessFlags = NULL;
	dstDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	dstDesc.Width = bbDesc.Width;
	dstDesc.Height = bbDesc.Height;
	dstDesc.MipLevels = 1;
	dstDesc.MiscFlags = NULL;
	dstDesc.SampleDesc.Count = 1;
	dstDesc.SampleDesc.Quality = 0;
	dstDesc.Usage = D3D11_USAGE_DEFAULT;

	hr = GraphicsContext::Device->CreateTexture2D(&dstDesc, NULL, &depthStencilTexture);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create depth stencil texture!", L"Startup Error", MB_OK | MB_ICONERROR);
		this->~Renderer();
		throw std::exception("Depth Stencil creation failed");
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	dsvDesc.Texture2D.MipSlice = 0;

	hr = GraphicsContext::Device->CreateDepthStencilView(depthStencilTexture, &dsvDesc, &p_DepthStencil);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create depth stencil view!", L"Startup Error", MB_OK | MB_ICONERROR);
		this->~Renderer();
		throw std::exception("Depth Stencil creation failed");
	}

	depthStencilTexture->Release();
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = NULL;
	dsDesc.StencilWriteMask = NULL;
	
	ID3D11DepthStencilState* depthStencilState;
	hr = GraphicsContext::Device->CreateDepthStencilState(&dsDesc, &depthStencilState);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create depth stencil state!", L"Startup Error", MB_OK | MB_ICONERROR);
		this->~Renderer();
		throw std::exception("Depth Stencil creation failed");
	}

	GraphicsContext::Context->OMSetDepthStencilState(depthStencilState, 0);
	depthStencilState->Release();

	m_Scene = new Scene;

	GraphicsContext::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0.f;
	vp.TopLeftY = 0.f;
	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;
	vp.Width = (float) bbDesc.Width;
	vp.Height = (float) bbDesc.Height;

	GraphicsContext::Context->RSSetViewports(1, &vp);
}

Renderer::~Renderer()
{
	if (p_RenderTarget) p_RenderTarget->Release();
	if (p_DepthStencil) p_DepthStencil->Release();

	delete m_Scene;
}

void Renderer::Render(float deltaTime)
{
	float color[] = { 0.11f, 0.18f, 0.96f, 1.f };
	GraphicsContext::Context->ClearRenderTargetView(p_RenderTarget, color);

	GraphicsContext::Context->OMSetRenderTargets(1, &p_RenderTarget, p_DepthStencil);
}

void Renderer::Resize()
{
	if (p_RenderTarget) p_RenderTarget->Release();
	if (p_DepthStencil) p_DepthStencil->Release();

	HRESULT hr = GraphicsContext::SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to resize swap chain!", L"Runtime Error", MB_OK | MB_ICONERROR);
	}

	ID3D11Texture2D* backBuffer;
	hr = GraphicsContext::SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to get window back buffer!", L"Runtime Error", MB_OK | MB_ICONERROR);
		this->~Renderer();
		throw std::exception("Render Target creation failed");
	}

	hr = GraphicsContext::Device->CreateRenderTargetView(backBuffer, NULL, &p_RenderTarget);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create Render Target View!", L"Runtime Error", MB_OK | MB_ICONERROR);
		this->~Renderer();
		throw std::exception("Render Target creation failed");
	}
	
	D3D11_TEXTURE2D_DESC bbDesc;
	backBuffer->GetDesc(&bbDesc);
	backBuffer->Release();

	ID3D11Texture2D* depthStencilTexture;
	D3D11_TEXTURE2D_DESC dstDesc;
	ZeroMemory(&dstDesc, sizeof(D3D11_TEXTURE2D_DESC));
	dstDesc.ArraySize = 1;
	dstDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dstDesc.CPUAccessFlags = NULL;
	dstDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dstDesc.Width = bbDesc.Width;
	dstDesc.Height = bbDesc.Height;
	dstDesc.MipLevels = 1;
	dstDesc.MiscFlags = NULL;
	dstDesc.SampleDesc.Count = 1;
	dstDesc.SampleDesc.Quality = 0;
	dstDesc.Usage = D3D11_USAGE_DEFAULT;

	hr = GraphicsContext::Device->CreateTexture2D(&dstDesc, NULL, &depthStencilTexture);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create depth stencil texture!", L"Startup Error", MB_OK | MB_ICONERROR);
		this->~Renderer();
		throw std::exception("Depth Stencil creation failed");
	}

	hr = GraphicsContext::Device->CreateDepthStencilView(depthStencilTexture, NULL, &p_DepthStencil);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create depth stencil view!", L"Startup Error", MB_OK | MB_ICONERROR);
		this->~Renderer();
		throw std::exception("Depth Stencil creation failed");
	}

	depthStencilTexture->Release();

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0.f;
	vp.TopLeftY = 0.f;
	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;
	vp.Width = (float) bbDesc.Width;
	vp.Height = (float) bbDesc.Height;

	GraphicsContext::Context->RSSetViewports(1, &vp);

	GraphicsContext::Context->Flush();
}
