#pragma once
//#include <d3d11.h>
//#include <d3dx11.h>
#include"D3Device.h"
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dx11.lib")
#include"SceneBase.h"
#include"SceneFactory.h"

//シーンの更新とフェードインアウトを行うクラス
class SceneManager:public Singleton<SceneManager>
{
private:
	friend class Singleton<SceneManager>;

	shared_ptr<SceneFactory>	m_pFactory;
	unique_ptr<SceneBase>		m_pScene;
	shared_ptr<Sprite>			m_pFadeTex;
	shared_ptr<Camera>			m_pCamera;
	shared_ptr<D3DRenderer>		m_pRenderer;
	DWORD SceneMsg;


	SceneManager();
	SceneManager(const SceneManager&) {};

public:
	bool IsLoading;
	bool IsFading;
	bool IsChange;
	unique_ptr<SpriteObject>	m_pFadeSprite;

	~SceneManager();
	void Init(shared_ptr<SceneFactory> pFactory,HWND hWnd);
	void SceneUpdate();
	void CreateFadeSprite(unique_ptr<SpriteObject>&);
	void Render();
	void FadeIn();
	void FadeOut();
	//void Save();
	void Load(DWORD SceneName);
	void Release();
};