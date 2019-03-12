#include "stdafx.h"
#include "GameScene.h"


GameScene::GameScene()
{
}


GameScene::~GameScene()
{
}

void GameScene::Load(shared_ptr<D3DRenderer> pRenderer)
{
	m_pRenderer = pRenderer;
	Mesh = make_unique<MeshObject>();
	Mesh->Create("../ShaderPractice/Resource/Kouhai_chan_anim.fbx",m_pRenderer->GetDevice());
}

void GameScene::Init()
{
	//ƒJƒƒ‰
	m_Camera.SetProjMat(D3DXToRadian(45.0f), 640 / 320, 0.1f, 10000.0f);
	m_Camera.SetViewMat(XMFLOAT3(0.0f, 0.8f, -10.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.8f, 10.0f));
	m_Camera.SetTranslateW(XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 0.0f)));
}

SCENE_MESSAGE GameScene::Update(DWORD* pNextScene)
{
	Mesh->m_WorldMat.RotationQuatanion(XMQuaternionRotationRollPitchYaw(0.0f, 0.01f, 0.0f));
	return SCENE_LOOP;
}

void GameScene::Render()
{	
	Mesh->Draw(m_Camera,m_Light,m_pRenderer->GetDevContext());
}

void GameScene::Release()
{}
