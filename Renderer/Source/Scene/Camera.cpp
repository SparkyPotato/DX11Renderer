#include "Camera.h"

Camera::Camera(ProjectionMode mode, float aspectRatio, float nearPlane, float farPlane)
	: m_AspectRatio(aspectRatio), m_NearPlane(nearPlane), m_FarPlane(farPlane), m_Mode(mode)
{
	if (mode == ProjectionMode::Perspective)
		m_Projection = DirectX::XMMatrixPerspectiveLH(aspectRatio * 2.f, 2.f, nearPlane, farPlane);
	else
		m_Projection = DirectX::XMMatrixOrthographicLH(aspectRatio * 2.f, 2.f, nearPlane, farPlane);

	m_ViewProjection = m_Projection;
}

void Camera::SetPosition(DirectX::XMVECTOR position)
{
	m_Position = position;
	auto transformation = DirectX::XMMatrixRotationRollPitchYawFromVector(m_Rotation) * DirectX::XMMatrixTranslationFromVector(m_Position);

	m_ViewProjection = DirectX::XMMatrixInverse(nullptr, transformation) * m_Projection;
}

void Camera::SetRotation(DirectX::XMVECTOR rotation)
{
	m_Rotation = rotation;
	auto transformation = DirectX::XMMatrixRotationRollPitchYawFromVector(m_Rotation) * DirectX::XMMatrixTranslationFromVector(m_Position);

	m_ViewProjection = DirectX::XMMatrixInverse(nullptr, transformation) * m_Projection;
}

void Camera::Resize(float aspectRatio)
{
	m_AspectRatio = aspectRatio;

	if (m_Mode == ProjectionMode::Perspective)
		m_Projection = DirectX::XMMatrixPerspectiveLH(aspectRatio * 2.f, 2.f, m_NearPlane, m_FarPlane);
	else
		m_Projection = DirectX::XMMatrixOrthographicLH(aspectRatio * 2.f, 2.f, m_NearPlane, m_FarPlane);

	auto transformation = DirectX::XMMatrixRotationRollPitchYawFromVector(m_Rotation) * DirectX::XMMatrixTranslationFromVector(m_Position);
	m_ViewProjection = DirectX::XMMatrixInverse(nullptr, transformation) * m_Projection;
}
