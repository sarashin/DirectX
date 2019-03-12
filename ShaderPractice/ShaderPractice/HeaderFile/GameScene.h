#pragma once
#include "SceneBase.h"
class GameScene :public SceneBase
{

	unique_ptr<MeshObject>	Mesh;
	shared_ptr<D3DRenderer> m_pRenderer;
	Camera					m_Camera;
	vector<Light>			m_Light;

public:
	GameScene();
	~GameScene();

	void Load(shared_ptr<D3DRenderer> pRenderer);
	void Init();
	SCENE_MESSAGE Update(DWORD* pNextScene);
	void Render();
	void Release();
};

