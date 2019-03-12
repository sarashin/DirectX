#include "stdafx.h"
#include "SceneManager.h"
#include "MeshObjectFactory.h"
#include "GameScene.h"
#define D3DX_PI ((FLOAT) 3.141592654f) 
#define D3DXToRadian( degree ) ((degree) * (D3DX_PI / 180.0f))

std::mutex g_mutex;


SceneManager::SceneManager()
{
	
}


SceneManager::~SceneManager()
{
	Release();
}

void SceneManager::CreateFadeSprite(unique_ptr<SpriteObject>& pSprite)
{
	pSprite = make_unique<SpriteObject>();
	pSprite->SetMeshObject(m_pFadeTex);
	pSprite->IsAppear = true;
	pSprite->GetMeshObject()->m_Material.Alpha = 0.0f;
	pSprite->Update();
}

void SceneManager::Init(shared_ptr<SceneFactory> pFactory,HWND hWnd)
{
	m_pFactory = pFactory;
	m_pScene = make_unique<GameScene>();
	SceneMsg = SCENE_TITLE;

	m_pRenderer = make_shared<D3DRenderer>();
	m_pRenderer->CreateDevice(hWnd);

	MeshFactory::GetInstance().Init(m_pRenderer);

	m_pFactory->GetFirstScene(m_pScene);
	m_pScene->Load(m_pRenderer);
	m_pScene->Init();
}

void SceneManager::SceneUpdate()
{
	/*if (IsLoading == false)
	{*/
		DWORD SceneState = m_pScene->Update(&SceneMsg);
		/*if (SceneState == SCENE_RELOAD)
		{
			CreateFadeSprite(m_pFadeSprite);
			IsLoading = true;
			IsFading = true;
			IsChange = false;
		}
		else if (SceneState == SCENE_CHANGE)
		{
			CreateFadeSprite(m_pFadeSprite);
			IsLoading = true;
			IsFading = true;
			IsChange = true;
		}
		*/
		
	/*}
	else
	{
		Render();
	}*/
}

void SceneManager::FadeIn()
{
	if (m_pFadeSprite->GetMeshObject()->m_Material.Alpha >= 1.0)
	{
		m_pFadeSprite->GetMeshObject()->m_Material.Alpha = 1.0;
		IsFading = false;
		if (IsChange)
			Load(SceneMsg);
		else
			m_pScene->Init();

			m_pFadeSprite->Update();
		return;
	}

	m_pFadeSprite->GetMeshObject()->m_Material.Alpha += 0.01f;
	return;
}

void SceneManager::Load(DWORD SceneName)
{
	m_pFactory->GetNextScene(m_pScene, SceneName);
}

void SceneManager::FadeOut()
{
	if (m_pFadeSprite->GetMeshObject()->m_Material.Alpha <= 0.0)
	{
		m_pFadeSprite->GetMeshObject()->m_Material.Alpha = 0.0;
		m_pFadeSprite->Release();
		IsLoading = false;
		return;
	}

	m_pFadeSprite->GetMeshObject()->m_Material.Alpha -= 0.01f;
	return;
}

void SceneManager::Render()
{
	m_pRenderer->RenderStart();

	m_pScene->Render();

	m_pRenderer->RenderEnd();
}

void SceneManager::Release()
{
	m_pFactory.reset();
	m_pFactory = NULL;

	m_pScene.release();
	m_pScene = NULL;
}