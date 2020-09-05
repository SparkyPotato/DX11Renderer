#pragma once
#include <DirectXMath.h>

enum class ProjectionMode
{
	Perspective, Orthographic
};

class Camera
{
public:
	Camera(ProjectionMode mode, float aspectRatio, float nearPlane, float farPlane);

	void SetPosition(DirectX::XMVECTOR position);
	void SetRotation(DirectX::XMVECTOR rotation);

	void Resize(float aspectRatio);

	DirectX::XMMATRIX GetViewProjection() { return m_ViewProjection; }

private:
	DirectX::XMMATRIX m_Projection;

	DirectX::XMVECTOR m_Position;
	DirectX::XMVECTOR m_Rotation;

	DirectX::XMMATRIX m_ViewProjection;

	ProjectionMode m_Mode;
	float m_AspectRatio, m_NearPlane, m_FarPlane;
};
