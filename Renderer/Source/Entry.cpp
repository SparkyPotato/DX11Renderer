#include <Windows.h>
#include <d3d11.h>
#include <wrl.h>

#pragma comment(lib, "d3d11.lib")

#include "Primitives/GraphicsContext.h"
#include "Renderer/Renderer.h"

// The Renderer class is the main application class, so making it static a  allows for the Window Procedure to easily access it for events. Not very good design,
// but we make sure it is not exported to other translation units
static Renderer* GRenderer = nullptr;
// Again, static global variable which keeps the application running
static bool IsRunning = true;

// Forward declaring the Window Procedure so WinMain can be on the top
static LRESULT WINAPI WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

// --------------- Entry Point ---------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	// Begin Window Class
	WNDCLASSEX windowClass = { 0 };
	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_CLASSDC;
	windowClass.lpfnWndProc = WindowProc; // Our Window Procedure declared above
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = NULL;
	windowClass.hIconSm = NULL;
	windowClass.hCursor = NULL;
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = L"Window"; // Window class name is "Window" because I didn't know what to name it

	if (RegisterClassEx(&windowClass) == 0)
	{
		// If we failed to register the window class, pop up a message box for the user to know, and quit the application by returning -1,
		// to show that it did not exit gracefully. We cannot continue because the window class is required to create windows
		MessageBox(NULL, L"Failed to register window class!", L"Startup Error", MB_OK | MB_ICONERROR);
		return -1;
	}

	// End Window Class

	// Begin Window Creation

	HWND window = CreateWindow
	(
		L"Window", // Name of the window class
		L"DirectX 11 Renderer", // Name of the window
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		1600, 900, // Default size of the window
		NULL, NULL,
		hInstance,
		NULL
	);

	if (window == NULL)
	{
		// If the window creation fails, show a message box and exit
		MessageBox(NULL, L"Failed to create window!", L"Startup Error", MB_OK | MB_ICONERROR);
		return -1;
	}

	// Creating the window doesn't actually show it to the user, we need to manually call ShowWindow
	ShowWindow(window, nShowCmd);

	// End Window Creation

	try
	{
		// Initializes DirectX 11
		GraphicsContext::Init(window);
	}
	catch (...)
	{
		// Catch any exceptions thrown and end execution
		return -1;
	}

	// We initialize all variables for tick delta calculation
	LARGE_INTEGER frequency, lastTickTime, currentTickTime;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&lastTickTime);

	try
	{
		// Create the renderer
		GRenderer = new Renderer;
	}
	catch (...)
	{
		// End execution on any exceptions
		return -1;
	}

	// Varible the keeps track of delta time
	float deltaTime;

	MSG message;
	// This is the main loop of the application and continues till the window is closed
	while (IsRunning)
	{
		// Windows event handler to ensure our application responds to window events
		while (PeekMessage(&message, window, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		// Get the time before the frame
		QueryPerformanceCounter(&currentTickTime);
		// Calculate the delta time (in seconds)
		deltaTime = (float)currentTickTime.QuadPart - (float)lastTickTime.QuadPart;
		deltaTime /= frequency.QuadPart;

		// Render
		GRenderer->Render(deltaTime);

		// Present the rendered frame to the user
		GraphicsContext::SwapChain->Present(0, 0);

		// Get the time after the tick
		QueryPerformanceCounter(&lastTickTime);
	}

	// Delete the renderer
	delete GRenderer;
	// Set it to nullptr so if the Window Procedure tries to access it, we don't crash
	GRenderer = nullptr;

	// Destroy the window before exiting
	DestroyWindow(window);

	return 0;
}

// Our Window Procedure
static LRESULT WINAPI WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CLOSE:
	{
		// If the window is closed, cause the application to exit out of the tick loop
		IsRunning = false;
		return 0;
	}
	case WM_SIZE:
	{
		// When the window is resized, tell the renderer to resize the buffers
		if (GRenderer) GRenderer->Resize();
		break;
	}
	}

	// Default Windows Procedure so we don't have to respond to every single event
	return DefWindowProc(window, message, wParam, lParam);
}
