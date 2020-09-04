#include <exception>

#include "GraphicsContext.h"

#include "Buffer.h"
#include "Shader.h"

Microsoft::WRL::ComPtr<ID3D11Device> GraphicsContext::Device = nullptr;
Microsoft::WRL::ComPtr<ID3D11DeviceContext> GraphicsContext::Context = nullptr;
Microsoft::WRL::ComPtr<IDXGISwapChain> GraphicsContext::SwapChain = nullptr;

VertexBuffer* GraphicsContext::m_LastVertexBuffer = nullptr;
VertexBuffer* GraphicsContext::m_CurrentVertexBuffer = nullptr;
VertexShader* GraphicsContext::m_CurrentVertexShader = nullptr;

void GraphicsContext::Init(HWND window)
{
	DXGI_SWAP_CHAIN_DESC sDesc;
	ZeroMemory(&sDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	sDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sDesc.BufferDesc.Width = 0;
	sDesc.BufferDesc.Height = 0;
	sDesc.BufferDesc.RefreshRate.Numerator = 0;
	sDesc.BufferDesc.RefreshRate.Denominator = 0;
	sDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
	sDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
	sDesc.SampleDesc.Count = 1;
	sDesc.SampleDesc.Quality = 0;
	sDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sDesc.BufferCount = 2;
	sDesc.OutputWindow = window;
	sDesc.Windowed = true;
	sDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sDesc.Flags = NULL;

	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_11_1 };
	HRESULT hr = D3D11CreateDeviceAndSwapChain
	(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		featureLevels,
		sizeof(featureLevels) / sizeof(D3D_FEATURE_LEVEL),
		D3D11_SDK_VERSION,
		&sDesc,
		&SwapChain,
		&Device,
		NULL,
		&Context
	);

	if (FAILED(hr))
	{
		MessageBox(window, L"Failed to initialize Direct3D 11!", L"Startup Error", MB_OK | MB_ICONERROR);
		throw std::exception("DirectX 11 initialization fail");
	}
}

void GraphicsContext::BindVertexBuffer(VertexBuffer* buffer)
{
	if (m_CurrentVertexBuffer == buffer) return;

	m_LastVertexBuffer = m_CurrentVertexBuffer;
	m_CurrentVertexBuffer = buffer;
	
	ID3D11Buffer* d3dbuffer;
	UINT stride, offset;

	if (buffer)
	{
		d3dbuffer = buffer->GetBuffer();
		stride = buffer->GetLayout().stride;
	}
	else
	{
		d3dbuffer = nullptr;
		stride = 0;
	}
	offset = 0;

	InputLayoutSetup();

	Context->IASetVertexBuffers(0, 1, &d3dbuffer, &stride, &offset);
}

void GraphicsContext::BindVertexShader(VertexShader* shader)
{
	m_CurrentVertexShader = shader;

	ID3D11VertexShader* d3dshader;

	if (shader)
	{
		d3dshader = shader->GetShader();
	}
	else
	{
		d3dshader = nullptr;
	}

	InputLayoutSetup();

	Context->VSSetShader(d3dshader, NULL, 0);
}

void GraphicsContext::InputLayoutSetup()
{
	if (!m_CurrentVertexShader || !m_CurrentVertexBuffer) return;

	if (!m_LastVertexBuffer || m_LastVertexBuffer->GetLayout() != m_CurrentVertexBuffer->GetLayout())
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout;

		for (auto& element : m_CurrentVertexBuffer->GetLayout().elements)
		{
			DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;

			switch (element.type)
			{
			case ElementDataType::float1: format = DXGI_FORMAT_R32_FLOAT;
			case ElementDataType::float2: format = DXGI_FORMAT_R32G32_FLOAT;
			case ElementDataType::float3: format = DXGI_FORMAT_R32G32B32_FLOAT;
			case ElementDataType::float4: format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}

			layout.push_back({ element.semantic.c_str(), 0, format, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
		}

		ID3D11InputLayout* iLayout;
		HRESULT hr = Device->CreateInputLayout
		(
			layout.data(), (UINT) layout.size(), 
			m_CurrentVertexShader->GetBlob()->GetBufferPointer(), m_CurrentVertexShader->GetBlob()->GetBufferSize(), 
			&iLayout
		);

		if (FAILED(hr))
		{
			MessageBox(NULL, L"Failed to create Input Layout!", L"Runtime Error", MB_OK | MB_ICONERROR);
			return;
		}

		Context->IASetInputLayout(iLayout);
		iLayout->Release();
	}
}

