#include <Windows.h>
#include <d3d11.h>
#include <wrl.h>

#pragma comment(lib, "d3d11.lib")

#include "examples/imgui_impl_win32.h"
#include "examples/imgui_impl_dx11.h"

#include "Primitives/GraphicsContext.h"
#include "Renderer/Renderer.h"

// The Renderer class is the main application class, so making it static a  allows for the Window Procedure to easily access it for events. Not very good design,
// but we make sure it is not exported to other translation units
static Renderer* GRenderer = nullptr;
// Again, static global variable which keeps the application running
static bool IsRunning = true;

// Forward declaring the Window Procedure so WinMain can be on the top
static LRESULT WINAPI WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

// Declaring the ImGui Window Procedure so we can call it in ours
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

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

	// Setup ImGui
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.DisplaySize = { 100.f, 100.f };

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.f;
		style.Colors[ImGuiCol_WindowBg].w = 1.f;
	}

	// Set the window style.
	style.ChildRounding = 0.f;
	style.GrabRounding = 3.f;
	style.WindowRounding = 3.f;
	style.ScrollbarRounding = 3.f;
	style.FrameRounding = 3.f;
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

	ImGui_ImplWin32_Init(window);

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

	// Variable that keeps track of delta time
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

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		GRenderer->RenderGui();

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// Update windows for ImGui viewports.
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();

		// Present the rendered frame to the user
		GraphicsContext::SwapChain->Present(0, 0);

		// Get the time after the tick
		QueryPerformanceCounter(&lastTickTime);
	}

	// Delete the renderer
	delete GRenderer;
	// Set it to nullptr so if the Window Procedure tries to access it, we don't crash
	GRenderer = nullptr;

	GraphicsContext::DeInit();
	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext();

	// Destroy the window before exiting
	DestroyWindow(window);

	return 0;
}

// Our Window Procedure
static LRESULT WINAPI WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Pass on all events to ImGui
	ImGui_ImplWin32_WndProcHandler(window, message, wParam, lParam);

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
