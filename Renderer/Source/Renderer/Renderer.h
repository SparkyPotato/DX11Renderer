#pragma once
#include "Primitives/GraphicsContext.h"
#include "Primitives/Buffer.h"
#include "Primitives/Shader.h"

#include "Scene/Camera.h"

class Scene;

class Renderer
{
public:
	Renderer();
	~Renderer();

	void Render(float deltaTime);
	void RenderGui();

	void Resize();

private:
	ID3D11RenderTargetView* p_RenderTarget = nullptr;
	ID3D11DepthStencilView* p_DepthStencil = nullptr;

	struct ObjectBuffer
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX worldViewProjection;
	};
	ConstantBuffer* m_ObjectBuffer;
	ObjectBuffer m_ObjectData;

	VertexShader* m_UnlitVS;
	PixelShader* m_UnlitSolidPS;

	Scene* m_Scene = nullptr;

	Camera m_MainCamera;

	bool m_IsSceneOpen = false;

	bool m_IsCameraOpen = true;
	float m_CameraPosition[3];
};
