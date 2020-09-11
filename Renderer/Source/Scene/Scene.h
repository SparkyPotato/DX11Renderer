#pragma once
#include <shobjidl.h>
#include <vector>

#include "Object.h"

struct SceneStats
{
	unsigned int objects = 0;
	unsigned int vertices = 0;
	unsigned int triangles = 0;
};

class Scene
{
public:
	Scene(bool* isOpen);
	~Scene();

	void AddObject();
	void AddObjectFromFile(std::string name, std::string filePath);

	void DrawObjects();

	std::vector<Object>& GetObjects() { return m_Objects; }
	const SceneStats& GetStats() { return m_Stats; }

private:
	void DrawProperties();
	void SetCurrentObject(Object* object);

	bool* m_IsOpen;
	bool m_ShowName = false;
	char m_NameBuf[512];
	IFileOpenDialog* p_FileOpen;
	std::string m_OpenedFilePath;

	SceneStats m_Stats;

	std::vector<Object> m_Objects;
	Object* p_CurrentObject = nullptr;
	float m_ObjectPosition[3];
	float m_ObjectRotation[3];
};
