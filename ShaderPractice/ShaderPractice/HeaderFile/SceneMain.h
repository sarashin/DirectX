#pragma once
#include "SceneBase.h"
class SceneMain :public SceneBase
{
public:
	SceneMain();
	~SceneMain();

	Camera					m_Camera;
	shared_ptr<MeshObject>	m_Mesh;


	void Load();
	void Init();
	SCENE_MESSAGE Update(DWORD* pNextScene);
	void Render();
	void Release();
};

