#pragma once
#include <shobjidl.h>
#include <vector>

#include "Object.h"

class Scene
{
public:
	Scene(bool* isOpen);
	~Scene();

	void AddObject();
	void AddObjectFromFile(std::string name, std::string filePath);

	void DrawObjects();

	const std::vector<Object>& GetObjects() { return m_Objects; }

private:
	bool* m_IsOpen;
	bool m_ShowName = false;
	char m_NameBuf[512];
	IFileOpenDialog* p_FileOpen;
	std::string m_OpenedFilePath;

	std::vector<Object> m_Objects;
};
