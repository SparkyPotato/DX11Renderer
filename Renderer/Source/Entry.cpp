#include <Windows.h>
#include <d3d11.h>
#include <wrl.h>

#pragma comment(lib, "d3d11.lib")

#include "Renderer.h"

static Renderer* GRenderer = nullptr;
static bool IsRunning = true;

LRESULT WINAPI WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CLOSE:
	{
		IsRunning = false;
		return 0;
	}
	case WM_SIZE:
	{
		if (GRenderer) GRenderer->Resize();
		break;
	}
	}

	return DefWindowProc(window, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	WNDCLASSEX windowClass = { 0 };
	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_CLASSDC;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = NULL;
	windowClass.hIconSm = NULL;
	windowClass.hCursor = NULL;
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = L"Window";

	if (RegisterClassEx(&windowClass) == 0)
	{
		MessageBox(NULL, L"Failed to register window class!", L"Startup Error", MB_OK | MB_ICONERROR);
		return -1;
	}

	HWND window = CreateWindow
	(
		L"Window",
		L"DirectX 11 Renderer",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		1600, 900,
		NULL, NULL,
		hInstance, 
		NULL
	);

	if (window == NULL)
	{
		MessageBox(NULL, L"Failed to create window!", L"Startup Error", MB_OK | MB_ICONERROR);
		return -1;
	}

	ShowWindow(window, nShowCmd);

	Microsoft::WRL::ComPtr<ID3D11Device> p_Device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> p_Context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> p_SwapChain;

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
		&p_SwapChain,
		&p_Device,
		NULL,
		&p_Context
	);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to initialize Direct3D 11!", L"Startup Error", MB_OK | MB_ICONERROR);
		return -1;
	}

	LARGE_INTEGER frequency, lastTickTime, currentTickTime;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&lastTickTime);

	try
	{
		GRenderer = new Renderer(p_Device.Get(), p_Context.Get(), p_SwapChain.Get());
	}
	catch (...)
	{
		return -1;
	}

	float deltaTime;

	MSG message;
	while (IsRunning)
	{
		while (PeekMessage(&message, window, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		QueryPerformanceCounter(&currentTickTime);
		deltaTime = (float) currentTickTime.QuadPart - (float) lastTickTime.QuadPart;
		deltaTime /= frequency.QuadPart;

		GRenderer->Render(deltaTime);

		p_SwapChain->Present(0, 0);

		QueryPerformanceCounter(&lastTickTime);
	}

	delete GRenderer;
	GRenderer = nullptr;

	return 0;
}
