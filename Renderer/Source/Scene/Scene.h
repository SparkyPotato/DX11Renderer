#pragma once
#include <shobjidl.h>
#include <vector>
#include "Object.h"

class Scene
{
public:
	Scene();
	~Scene();

	void AddObject();

private:
	IFileOpenDialog* p_FileOpen;

	std::vector<Object> m_Objects;
};
