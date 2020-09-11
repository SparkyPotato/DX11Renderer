#pragma once
#include "Primitives/GraphicsContext.h"
#include "Primitives/Buffer.h"
#include "Primitives/Shader.h"

#include "Scene/Camera.h"

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

	struct alignas(16) LightBuffer
	{
		DirectX::XMFLOAT4 lightPosition;
		DirectX::XMFLOAT3 ambient;
		float ambientIntensity;
		DirectX::XMFLOAT3 color;
		float intensity;
		float attConstant = 1.0f;
		float attLinear = 0.045f;
		float attQuadratic = 0.0075f;
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

	bool m_IsStatsOpen = false;
	float m_DeltaTime = 0.f;
};
