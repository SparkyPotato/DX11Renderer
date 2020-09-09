#include <Windows.h>
#include "imgui.h"

#include "Scene.h"

Scene::Scene(bool* isOpen)
	: m_IsOpen(isOpen)
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to initialize COM~", L"Startup Error", MB_OK | MB_ICONERROR);
		throw std::exception("COM initialization failed.");
	}

	hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&p_FileOpen));
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create file open dialog!", L"Startup Error", MB_OK | MB_ICONERROR);
		throw std::exception("File open dialog creation failed.");
	}

	p_FileOpen->SetTitle(L"Import 3D Object");
	p_FileOpen->SetOkButtonLabel(L"Import");

	COMDLG_FILTERSPEC files[] =
	{
		{ L"All Files", L"*.dae; *.fbx; *.gltf; *.obj" },
		{ L"Collada (.dae)", L"*.dae" },
		{ L"FBX (.fbx)", L"*.fbx" },
		{ L"glTF 2 (.gltf)", L"*.gltf" },
		{ L"Wavefront (.obj)", L"*.obj" }
	};

	hr = p_FileOpen->SetFileTypes(sizeof(files) / sizeof(COMDLG_FILTERSPEC), files);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to set allowed file formats!", L"Startup Error", MB_OK | MB_ICONERROR);
		throw std::exception("File open dialog creation failed.");
	}

	memset(m_NameBuf, 0, sizeof(m_NameBuf) / sizeof(char));
}

Scene::~Scene()
{
	p_FileOpen->Release();
	CoUninitialize();
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

				m_OpenedFilePath = path;
				m_ShowName = true;
			}
			else
			{
				MessageBox(NULL, L"Failed to get file path while loading mesh!", L"Runtime Error", MB_OK | MB_ICONERROR);
			}

			item->Release();
		}
		else
		{
			MessageBox(NULL, L"Failed to get selected file!", L"Runtime Error", MB_OK | MB_ICONERROR);
		}
	}
}

void Scene::AddObjectFromFile(std::string name, std::string filePath)
{
	try
	{
		m_Objects.emplace_back(name, filePath);
	}
	catch (...) {}
}

void Scene::DrawObjects()
{
	if (*m_IsOpen)
	{
		if (ImGui::Begin("Scene", m_IsOpen))
		{
			if (ImGui::Button("+"))
			{
				AddObject();
			}

			ImGui::BeginChild("Objects");
			for (auto& object : m_Objects)
			{
				ImGui::Selectable(object.Name.c_str());

				if (ImGui::BeginPopupContextItem(nullptr, ImGuiPopupFlags_MouseButtonRight))
				{
					if (ImGui::Selectable("Delete"))
					{
						auto it = std::find(m_Objects.begin(), m_Objects.end(), object);
						m_Objects.erase(it);
						ImGui::EndPopup();
						break;
					}
					ImGui::EndPopup();
				}
			}
			ImGui::EndChild();

			ImGui::End();
		}
		else
		{
			ImGui::End();
		}

		if (m_ShowName)
		{
			ImGui::OpenPopup("Name");
		}

		if (ImGui::BeginPopup("Name"))
		{
			if (m_ShowName)
			{
				ImGui::SetKeyboardFocusHere();
				m_ShowName = false;
			}
			if (ImGui::InputTextWithHint("", "Name", m_NameBuf, sizeof(m_NameBuf), ImGuiInputTextFlags_EnterReturnsTrue))
			{
				if (!std::string(m_NameBuf).empty())
				{
					AddObjectFromFile(m_NameBuf, m_OpenedFilePath);
					memset(m_NameBuf, 0, sizeof(m_NameBuf));
				}
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
}
