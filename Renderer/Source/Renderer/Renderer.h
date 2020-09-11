#pragma once
#include "Primitives/GraphicsContext.h"
#include "Primitives/Buffer.h"
#include "Primitives/Shader.h"

#include "Scene/Camera.h"
#include "Scene/Scene.h"

class Scene;

struct RendererStats
{
	unsigned int drawCalls = 0;
};

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

	ConstantBuffer* m_MaterialBuffer;

	RendererStats m_Stats;

	struct LightBuffer
	{
		DirectX::XMFLOAT4 lightPosition;
		DirectX::XMFLOAT3 ambient = { 1.f, 1.f, 1.f };
		float ambientIntensity = 0.1f;
		DirectX::XMFLOAT3 color = { 1.f, 1.f, 1.f };
		float intensity = 1.f;
		float attConstant = 1.0f;
		float attLinear = 0.045f;
		float attQuadratic = 0.0075f;

	private:
		float p0;
	};
	ConstantBuffer* m_LightBuffer;
	LightBuffer m_LightData;

	struct ObjectBuffer
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX worldViewProjection;
	};
	ConstantBuffer* m_ObjectBuffer;
	ObjectBuffer m_ObjectData;

	struct CameraBuffer
	{
		DirectX::XMVECTOR cameraPosition;
	};
	ConstantBuffer* m_CameraBuffer;
	CameraBuffer m_CameraData;

	VertexShader* m_UnlitVS;

	PixelShader* m_UnlitSolidPS;
	PixelShader* m_LitSolidPS;

	Scene* m_Scene = nullptr;

	Camera m_MainCamera;

	bool m_IsSceneOpen = false;

	bool m_IsCameraOpen = true;
	float m_CameraPosition[3];
	float m_CameraRotation[2];

	bool m_IsLightOpen = false;
	Object m_Light;

	bool m_IsStatsOpen = false;
	float m_DeltaTime = 0.f;
};
