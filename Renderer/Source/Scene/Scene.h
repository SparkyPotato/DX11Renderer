#pragma once
#include <shobjidl.h>
#include <vector>

#include "Object.h"
#include "Primitives/Buffer.h"

struct SceneObject
{
	const DirectX::XMMATRIX worldMatrix;
	const VertexBuffer* vertexBuffer;
	const IndexBuffer* indexBuffer;
};

class Scene
{
public:
	Scene(bool* isOpen);
	~Scene();

	void AddObject();
	void AddObjectFromFile(std::string filePath);

	void DrawObjects();

	const std::vector<SceneObject>& GetSceneObjects() { return m_SceneObjects; }

private:
	bool* m_IsOpen;
	IFileOpenDialog* p_FileOpen;

	std::vector<Object> m_Objects;
	std::vector<SceneObject> m_SceneObjects;
	VertexLayout m_VertexLayout =
	{
		{ "POSITION", ElementDataType::float3 },
		{ "NORMAL", ElementDataType::float3 },
		{ "COLOR", ElementDataType::float4 },
		{ "TEXCOORD", ElementDataType::float2 }
	};
};
