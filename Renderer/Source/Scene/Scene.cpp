#include <Windows.h>
#include "imgui.h"

#include "Scene.h"

Scene::Scene(bool* isOpen)
	: m_IsOpen(isOpen)
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to initialize COM! Cannot proceed", L"Startup Error", MB_OK | MB_ICONERROR);
		throw std::exception("COM initialization failed.");
	}

	hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&p_FileOpen));
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create file open dialog! Cannot proceed", L"Startup Error", MB_OK | MB_ICONERROR);
		throw std::exception("File open dialog creation failed.");
	}

	p_FileOpen->SetTitle(L"Import 3D Object");
	p_FileOpen->SetOkButtonLabel(L"Import");
}

Scene::~Scene()
{
	p_FileOpen->Release();
	CoUninitialize();

	for (auto& sObj : m_SceneObjects)
	{
		delete sObj.vertexBuffer;
		delete sObj.indexBuffer;
	}
}

void Scene::AddObject()
{
	HRESULT hr = p_FileOpen->Show(NULL);

	if (SUCCEEDED(hr))
	{
		IShellItem* item;
		hr = p_FileOpen->GetResult(&item);

		if (SUCCEEDED(hr))
		{
			LPWSTR filePath;
			hr = item->GetDisplayName(SIGDN_FILESYSPATH, &filePath);

			if (SUCCEEDED(hr))
			{
				std::wstring temp(filePath);
				int size_needed = WideCharToMultiByte(CP_UTF8, 0, temp.c_str(), -1, NULL, 0, NULL, NULL);
				std::string path(size_needed, 0);
				WideCharToMultiByte(CP_UTF8, 0, temp.c_str(), (int)temp.size(), &path[0], size_needed, NULL, NULL);

				AddObjectFromFile(path);
			}
			else
			{
				MessageBox(NULL, L"Failed to get file path while loading texture!", L"Runtime Error", MB_OK | MB_ICONERROR);
			}

			item->Release();
		}
		else
		{
			MessageBox(NULL, L"Failed to get selected file!", L"Runtime Error", MB_OK | MB_ICONERROR);
		}
	}
}

void Scene::AddObjectFromFile(std::string filePath)
{
	try
	{
		Object obj(filePath, filePath);
		m_Objects.push_back(obj);

		SceneObject sObj =
		{
			obj.GetWorldMatrix(),
			new VertexBuffer(m_VertexLayout, BufferAccess::Static, obj.GetVertices().data(), obj.GetVertices().size()),
			new IndexBuffer(BufferAccess::Static, obj.GetIndices().data(), obj.GetIndices().size())
		};
		m_SceneObjects.push_back(sObj);
	}
	catch (...) {}
}

void Scene::DrawObjects()
{
	if (*m_IsOpen)
	{
		if (ImGui::Begin("Objects", m_IsOpen))
		{
			if (ImGui::Button("+"))
			{
				AddObject();
			}

			for (auto& object : m_Objects)
			{
				ImGui::Text("%s", object.Name.c_str());
			}

			ImGui::End();
		}
		else
		{
			ImGui::End();
		}
	}
}
