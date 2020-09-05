#pragma once
#include "Primitives/GraphicsContext.h"
#include "Primitives/Buffer.h"

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

	struct CameraBuffer
	{
		DirectX::XMMATRIX viewProjection;
	};
	ConstantBuffer* m_CameraBuffer;
	CameraBuffer m_CameraData;

	Scene* m_Scene = nullptr;

	Camera m_MainCamera;
};
