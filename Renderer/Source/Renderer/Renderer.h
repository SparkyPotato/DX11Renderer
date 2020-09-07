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

	struct MaterialBuffer
	{
		float ambient = 1.f;
		float specular = 1.f;
		float diffuse = 1.f;
		float shininess = 1.f;
	};
	ConstantBuffer* m_MaterialBuffer;
	MaterialBuffer m_MaterialData;

	struct LightBuffer
	{
		DirectX::XMVECTOR lightPosition;
		DirectX::XMVECTOR ambient;
		DirectX::XMVECTOR specular;
		DirectX::XMVECTOR diffuse;
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

	bool m_IsLightOpen = false;
	float m_LightPosition[3];
	float m_AmbientColor[3];
	float m_AmbientIntensity;
	float m_SpecularColor[3];
	float m_SpecularIntensity;
	float m_DiffuseColor[3];
	float m_DiffuseIntensity;
};
