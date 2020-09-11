#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Object.h"

Object::Object(std::string name, std::string file)
	: Name(name), m_World(DirectX::XMMatrixIdentity()), m_Position(DirectX::XMVectorZero()), m_Rotation(DirectX::XMVectorZero())
{
	Assimp::Importer importer;
	importer.SetPropertyFloat("PP_GSN_MAX_SMOOTHING_ANGLE", 90.f);
	auto scene = importer.ReadFile(file, aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene == nullptr)
	{
		MessageBox(NULL, L"Failed to load mesh!", L"Runtime Error", MB_OK | MB_ICONERROR);
		throw 0;
	}
	auto mesh = scene->mMeshes[0];
	bool hasVertexColors = mesh->HasVertexColors(0);
	bool hasNormals = mesh->HasNormals();
	bool hasTexCoords = mesh->HasTextureCoords(0);

	m_Vertices.resize(mesh->mNumVertices);

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		m_Vertices[i].position.x = mesh->mVertices[i].x;
		m_Vertices[i].position.y = mesh->mVertices[i].y;
		m_Vertices[i].position.z = mesh->mVertices[i].z;

		if (hasVertexColors)
		{
			m_Vertices[i].color.r = mesh->mColors[0][i].r;
			m_Vertices[i].color.g = mesh->mColors[0][i].g;
			m_Vertices[i].color.b = mesh->mColors[0][i].b;
			m_Vertices[i].color.a = mesh->mColors[0][i].a;
		}
		else
		{
			m_Vertices[i].color.r = 1.f;
			m_Vertices[i].color.g = 1.f;
			m_Vertices[i].color.b = 1.f;
			m_Vertices[i].color.a = 1.f;
		}

		if (hasNormals)
		{
			m_Vertices[i].normal.x = mesh->mNormals[i].x;
			m_Vertices[i].normal.y = mesh->mNormals[i].y;
			m_Vertices[i].normal.z = mesh->mNormals[i].z;
		}
		else
		{
			m_Vertices[i].normal.x = 0.f;
			m_Vertices[i].normal.y = 0.f;
			m_Vertices[i].normal.z = 0.f;
		}

		if (hasTexCoords)
		{
			m_Vertices[i].texcoord.u = mesh->mTextureCoords[0][i].x;
			m_Vertices[i].texcoord.v = mesh->mTextureCoords[0][i].y;
		}
		else
		{
			m_Vertices[i].texcoord.u = 0.f;
			m_Vertices[i].texcoord.v = 0.f;
		}
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		if (mesh->mFaces[i].mNumIndices != 3) continue;

		m_Indices.push_back(mesh->mFaces[i].mIndices[0]);
		m_Indices.push_back(mesh->mFaces[i].mIndices[1]);
		m_Indices.push_back(mesh->mFaces[i].mIndices[2]);
	}

	m_VertexBuffer = new VertexBuffer(m_VertexLayout, BufferAccess::Static, m_Vertices.data(), (unsigned int) m_Vertices.size());
	m_IndexBuffer = new IndexBuffer(BufferAccess::Static, m_Indices.data(), (unsigned int) m_Indices.size());
}

Object::Object(Object&& other) noexcept
{
	m_VertexBuffer = other.m_VertexBuffer;
	other.m_VertexBuffer = nullptr;
	m_IndexBuffer = other.m_IndexBuffer;
	other.m_IndexBuffer = nullptr;

	m_Vertices = std::move(other.m_Vertices);
	m_Indices = std::move(other.m_Indices);
	Name = std::move(other.Name);
	m_Material = other.m_Material;

	m_World = other.m_World;
	m_Position = other.m_Position;
	m_Rotation = other.m_Rotation;
}

Object::Object(const Object& other)
{
	Name = other.Name;
}

Object::~Object() noexcept
{
	delete m_VertexBuffer;
	delete m_IndexBuffer;
}

void Object::CalculateMatrix()
{
	m_World =  DirectX::XMMatrixRotationRollPitchYawFromVector(m_Rotation) * DirectX::XMMatrixTranslationFromVector(m_Position);
}
