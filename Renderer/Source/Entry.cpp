#include <Windows.h>
#include <d3d11.h>
#include <wrl.h>

#pragma comment(lib, "d3d11.lib")

#include "Primitives/GraphicsContext.h"
#include "Renderer/Renderer.h"

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

	GraphicsContext::Init(window);

	LARGE_INTEGER frequency, lastTickTime, currentTickTime;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&lastTickTime);

	try
	{
		GRenderer = new Renderer;
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

		GraphicsContext::SwapChain->Present(0, 0);

		QueryPerformanceCounter(&lastTickTime);
	}

	delete GRenderer;
	GRenderer = nullptr;

	return 0;
}
