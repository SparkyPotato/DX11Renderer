#include <exception>

#include "GraphicsContext.h"

#include "examples/imgui_impl_dx11.h"

#include "Buffer.h"
#include "Shader.h"

Microsoft::WRL::ComPtr<ID3D11Device> GraphicsContext::Device = nullptr;
Microsoft::WRL::ComPtr<ID3D11DeviceContext> GraphicsContext::Context = nullptr;
Microsoft::WRL::ComPtr<IDXGISwapChain> GraphicsContext::SwapChain = nullptr;

const VertexBuffer* GraphicsContext::m_LastVertexBuffer = nullptr;
const VertexBuffer* GraphicsContext::m_CurrentVertexBuffer = nullptr;
const VertexShader* GraphicsContext::m_CurrentVertexShader = nullptr;

ConstantBuffer* GraphicsContext::m_BoundVSConstantBuffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT];
ID3D11Buffer* GraphicsContext::m_VSConstantBuffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT];
ConstantBuffer* GraphicsContext::m_BoundPSConstantBuffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT];
ID3D11Buffer* GraphicsContext::m_PSConstantBuffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT];

void GraphicsContext::Init(HWND window)
{
	// This describes the settings of the swap chain.
	// The swap chain contains the back buffer of the window, and is responsible for swapping the buffers and showing
	// the rendered image to the user every frame
	DXGI_SWAP_CHAIN_DESC sDesc;
	ZeroMemory(&sDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	sDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 8-bit per color, to give a total of 256 values for each color
	sDesc.BufferDesc.Width = 0; // This automatically infers the width and height from the window
	sDesc.BufferDesc.Height = 0; // ^
	sDesc.BufferDesc.RefreshRate.Numerator = 0; // We don't have any refresh rate limits
	sDesc.BufferDesc.RefreshRate.Denominator = 0; // ^
	sDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED; // Stretch the image while resizing the window
	sDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE; // Progressive scanline ordering, for those 1080p displays
	sDesc.SampleDesc.Count = 1; // No multisampling, no anti-aliasing
	sDesc.SampleDesc.Quality = 0; // ^
	sDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // The swap chain buffers are created to be used as render targets, so we can actually draw on them
	sDesc.BufferCount = 2; // 2 buffers, so we aren't drawing directly to the front buffer
	sDesc.OutputWindow = window; // Setting the swap chain to use our window created in the entry point
	sDesc.Windowed = true; // Start off in windowed mode
	sDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // Set the swap chain to flip the buffers on every present call, but discard the previous frame
	sDesc.Flags = NULL;

	// Create a device only using Direct3D 11 or 11.1
	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_11_1 };
	// Create the actual DirectX device, context, and swap chain
	HRESULT hr = D3D11CreateDeviceAndSwapChain
	(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE, // Make sure we use hardware acceleration
		NULL,
		D3D11_CREATE_DEVICE_DEBUG, // Create the device with the debug layer so we know when we make mistakes
		featureLevels,
		sizeof(featureLevels) / sizeof(D3D_FEATURE_LEVEL),
		D3D11_SDK_VERSION, // Use the SDK version installed on the system
		&sDesc,
		&SwapChain,
		&Device,
		NULL,
		&Context
	);

	if (FAILED(hr))
	{
		// If we fail, drop a message box as usual and throw an exception to stop application execution
		MessageBox(window, L"Failed to initialize Direct3D 11!", L"Startup Error", MB_OK | MB_ICONERROR);
		throw std::exception("DirectX 11 initialization fail");
	}

	// We set all bound constant buffers to nullptr because there's nothing bound
	for (int i = 0; i < D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT; i++)
	{
		m_BoundVSConstantBuffers[i] = nullptr;
		m_BoundPSConstantBuffers[i] = nullptr;
	}

	ImGui_ImplDX11_Init(Device.Get(), Context.Get());
}

void GraphicsContext::DeInit()
{
	ImGui_ImplDX11_Shutdown();
}

void GraphicsContext::BindVertexBuffer(const VertexBuffer* buffer)
{
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

void GraphicsContext::BindVertexShader(const VertexShader* shader)
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

void GraphicsContext::BindVSConstantBuffer(const ConstantBuffer* buffer, unsigned int slot)
{
	if (slot >= D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT)
	{
		MessageBox(NULL, L"Constant Buffer cannot be bound to the slot", L"Runtime Error", MB_OK | MB_ICONERROR);
	}

	if (m_BoundVSConstantBuffers[slot]) m_BoundVSConstantBuffers[slot]->SetUnboundSlot();

	m_BoundVSConstantBuffers[slot] = const_cast<ConstantBuffer*>(buffer);
	m_VSConstantBuffers[slot] = buffer->GetBuffer();

	Context->VSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, m_VSConstantBuffers);
}

void GraphicsContext::BindPSConstantBuffer(const ConstantBuffer* buffer, unsigned int slot)
{
	if (slot >= D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT)
	{
		MessageBox(NULL, L"Constant Buffer cannot be bound to the slot", L"Runtime Error", MB_OK | MB_ICONERROR);
	}

	if (m_BoundPSConstantBuffers[slot]) m_BoundPSConstantBuffers[slot]->SetUnboundSlot();

	m_BoundPSConstantBuffers[slot] = const_cast<ConstantBuffer*>(buffer);
	m_PSConstantBuffers[slot] = buffer->GetBuffer();

	Context->PSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, m_PSConstantBuffers);
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
			case ElementDataType::float1: format = DXGI_FORMAT_R32_FLOAT; break;
			case ElementDataType::float2: format = DXGI_FORMAT_R32G32_FLOAT; break;
			case ElementDataType::float3: format = DXGI_FORMAT_R32G32B32_FLOAT; break;
			case ElementDataType::float4: format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
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

