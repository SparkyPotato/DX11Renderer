#pragma once
#include "Primitives/GraphicsContext.h"
#include "Primitives/Buffer.h"
#include "Primitives/Shader.h"

class Scene;

class Renderer
{
public:
	Renderer();
	~Renderer();

	void Render(float deltaTime);

	void Resize();

private:
	ID3D11RenderTargetView* p_RenderTarget = nullptr;
	ID3D11DepthStencilView* p_DepthStencil = nullptr;

	Scene* m_Scene = nullptr;

	VertexBuffer* m_Buffer;
	VertexShader* m_VShader;
	PixelShader* m_PShader;
};
