#include <exception>
#include <d3d11.h>

#include "imgui.h"

#include "Renderer.h"
#include "Scene/Scene.h"
#include "Scene/Object.h"

Renderer::Renderer()
	: m_MainCamera(ProjectionMode::Perspective, 1.f, 2.5f, 1000.f)
{
	// Grab the back buffer from the swap chain
	ID3D11Texture2D* backBuffer;
	HRESULT hr = GraphicsContext::SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &backBuffer);
	if (FAILED(hr))
	{
		// Pop a message box and exit if failed
		MessageBox(NULL, L"Failed to get window back buffer!", L"Startup Error", MB_OK | MB_ICONERROR);
		this->~Renderer();
		throw std::exception("Render Target creation failed");
	}

	// Create a render target to draw to
	hr = GraphicsContext::Device->CreateRenderTargetView(backBuffer, NULL, &p_RenderTarget);
	if (FAILED(hr))
	{
		// Pop a message box and exit if failed
		MessageBox(NULL, L"Failed to create Render Target View!", L"Startup Error", MB_OK | MB_ICONERROR);
		this->~Renderer();
		throw std::exception("Render Target creation failed");
	}

	// Get the description of the back buffer so we can create a depth stencil with the same dimensions
	D3D11_TEXTURE2D_DESC bbDesc;
	backBuffer->GetDesc(&bbDesc);
	backBuffer->Release();

	// Create the depth stencil texture description.
	// A depth stencil is a texture used to store the Z-value of pixels to see if they are occluded by something in-front of them or not
	ID3D11Texture2D* depthStencilTexture;
	D3D11_TEXTURE2D_DESC dstDesc;
	ZeroMemory(&dstDesc, sizeof(D3D11_TEXTURE2D_DESC));
	dstDesc.ArraySize = 1; // There is only one texture to be created
	dstDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL; // Inform DirectX that the texture will be used as a depth stencil
	dstDesc.CPUAccessFlags = NULL; // We don't access it from the CPU
	dstDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	dstDesc.Width = bbDesc.Width; // Use the width and height of the back buffer
	dstDesc.Height = bbDesc.Height; // ^
	dstDesc.MipLevels = 1; // No mip levels because we don't need to downsample the depth stencil
	dstDesc.MiscFlags = NULL;
	dstDesc.SampleDesc.Count = 1; // No multisampling
	dstDesc.SampleDesc.Quality = 0;
	dstDesc.Usage = D3D11_USAGE_DEFAULT; // Default DirectX usage

	// Create the actual texture
	hr = GraphicsContext::Device->CreateTexture2D(&dstDesc, NULL, &depthStencilTexture);
	if (FAILED(hr))
	{
		// If we fail, give the error and exit
		MessageBox(NULL, L"Failed to create depth stencil texture!", L"Startup Error", MB_OK | MB_ICONERROR);
		this->~Renderer();
		throw std::exception("Depth Stencil creation failed");
	}

	// Create the view.
	// A view in DirectX is an interface required to interact with base resources
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	dsvDesc.Texture2D.MipSlice = 0;

	hr = GraphicsContext::Device->CreateDepthStencilView(depthStencilTexture, &dsvDesc, &p_DepthStencil);
	if (FAILED(hr))
	{
		// If we fail, drop the error and exit
		MessageBox(NULL, L"Failed to create depth stencil view!", L"Startup Error", MB_OK | MB_ICONERROR);
		this->~Renderer();
		throw std::exception("Depth Stencil creation failed");
	}

	// Release our reference to the depth stencil texture because we don't need it any more
	depthStencilTexture->Release();

	// This describes the depth testing the DirectX runtime will do
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dsDesc.DepthEnable = true; // Enables depth testing
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // Make sure we write to the depth stencil after a pixel is placed
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; // Everything with a lesser value or equal value is closer
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = NULL;
	dsDesc.StencilWriteMask = NULL;
	
	// Create the depth stencil state from the above description
	ID3D11DepthStencilState* depthStencilState;
	hr = GraphicsContext::Device->CreateDepthStencilState(&dsDesc, &depthStencilState);
	if (FAILED(hr))
	{
		// If we fail, drop the error
		MessageBox(NULL, L"Failed to create depth stencil state!", L"Startup Error", MB_OK | MB_ICONERROR);
		this->~Renderer();
		throw std::exception("Depth Stencil creation failed");
	}

	// Set the depth stencil state in the pipeline
	GraphicsContext::Context->OMSetDepthStencilState(depthStencilState, 0);
	// Release our reference to the state since we aren't touching it anymore
	depthStencilState->Release();

	// Tell the runtime that everything we render is going to be supplied as a list of triangles
	GraphicsContext::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create a viewport to tell how much of the render target we want to use
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0.f; // Top-left X and Y value of the viewport relative to the window
	vp.TopLeftY = 0.f; // ^
	vp.MinDepth = 0.f; // The minimum depth of the NDC space
	vp.MaxDepth = 1.f; // Maximum depth of the NDC space
	vp.Width = (float) bbDesc.Width; // Width and height of window so we use up the entire window
	vp.Height = (float) bbDesc.Height; // ^

	// Bind the viewport to the pipeline
	GraphicsContext::Context->RSSetViewports(1, &vp);

	m_ObjectBuffer = new ConstantBuffer(nullptr, sizeof(ObjectBuffer), ConstantBufferTarget::VertexShader);

	m_UnlitVS = new VertexShader(L"UnlitVS.cso");
	m_UnlitSolidPS = new PixelShader(L"UnlitSolidPS.cso");

	
	m_MainCamera.Resize((float)bbDesc.Width / (float)bbDesc.Height);
	m_Scene = new Scene(&m_IsSceneOpen);
}

Renderer::~Renderer()
{
	if (p_RenderTarget) p_RenderTarget->Release();
	if (p_DepthStencil) p_DepthStencil->Release();

	delete m_UnlitVS;
	delete m_UnlitSolidPS;

	delete m_ObjectBuffer;

	delete m_Scene;
}

void Renderer::Render(float deltaTime)
{
	// Clear the render target and depth stencil at the beginning of every frame so we don't have residue left over from the previous frame
	float color[] = { 0.11f, 0.18f, 0.96f, 1.f };
	GraphicsContext::Context->ClearRenderTargetView(p_RenderTarget, color);
	GraphicsContext::Context->ClearDepthStencilView(p_DepthStencil, D3D11_CLEAR_DEPTH, 1.f, 0);

	// Make sure we're rendering to our render target, because it could've been recreated on a resize event
	GraphicsContext::Context->OMSetRenderTargets(1, &p_RenderTarget, p_DepthStencil);

	m_UnlitVS->Bind();
	m_UnlitSolidPS->Bind();
	m_ObjectBuffer->Bind(0);

	for (auto& object : m_Scene->GetSceneObjects())
	{
		m_ObjectData.world = object.worldMatrix;
		m_ObjectData.worldViewProjection = object.worldMatrix * m_MainCamera.GetViewProjection();
		m_ObjectBuffer->Set(&m_ObjectData);
		object.vertexBuffer->Bind();
		object.indexBuffer->Bind();

		GraphicsContext::Context->DrawIndexed(object.indexBuffer->GetSize(), 0, 0);
	}
}

void Renderer::RenderGui()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Scene"))
		{
			ImGui::MenuItem("Camera", "", &m_IsCameraOpen);
			ImGui::MenuItem("Objects", "", &m_IsSceneOpen);

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	m_Scene->DrawObjects();

	if (m_IsCameraOpen)
	{
		if (ImGui::Begin("Camera Controls", &m_IsCameraOpen))
		{
			if (ImGui::DragFloat3("Position", m_CameraPosition, 0.01f, 0.005f, 0.002f, "%.3f", 1.f))
			{
				DirectX::XMFLOAT3 pos = { m_CameraPosition[0], m_CameraPosition[1], m_CameraPosition[2] };
				m_MainCamera.SetPosition(DirectX::XMLoadFloat3(&pos));
			}

			ImGui::End();
		}
		else
		{
			ImGui::End();
		}
	}
}

void Renderer::Resize()
{
	// If we currently have a render target or depth stencil, release references to them
	if (p_RenderTarget) p_RenderTarget->Release();
	if (p_DepthStencil) p_DepthStencil->Release();

	// Tell the swap chain to resize itself to the window
	HRESULT hr = GraphicsContext::SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	if (FAILED(hr))
	{
		// Tell the user we failed to resize
		MessageBox(NULL, L"Failed to resize swap chain!", L"Runtime Error", MB_OK | MB_ICONERROR);
	}

	// Recreate the back buffer, like in the constructor
	ID3D11Texture2D* backBuffer;
	hr = GraphicsContext::SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to get window back buffer!", L"Runtime Error", MB_OK | MB_ICONERROR);
		this->~Renderer();
		throw std::exception("Render Target creation failed");
	}

	// Recreate the render target
	hr = GraphicsContext::Device->CreateRenderTargetView(backBuffer, NULL, &p_RenderTarget);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create Render Target View!", L"Runtime Error", MB_OK | MB_ICONERROR);
		this->~Renderer();
		throw std::exception("Render Target creation failed");
	}
	
	// Recreate the depth stencil
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

	// Recreate the viewport for the new width and height
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0.f;
	vp.TopLeftY = 0.f;
	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;
	vp.Width = (float) bbDesc.Width;
	vp.Height = (float) bbDesc.Height;

	GraphicsContext::Context->RSSetViewports(1, &vp);

	// Flush all commands so we don't keep building up a list of commands when the user is resizing the window.
	// This helps us keep memory usage to a sane level, instead of it shooting up to around 1 GB for a long resize
	GraphicsContext::Context->Flush();

	m_MainCamera.Resize((float) bbDesc.Width / (float) bbDesc.Height);
}
