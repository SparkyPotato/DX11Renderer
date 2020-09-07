#include "Object.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <Windows.h>

Object::Object(std::string name, std::string file)
	: Name(name), m_World(DirectX::XMMatrixIdentity())
{
	Assimp::Importer importer;
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
}
