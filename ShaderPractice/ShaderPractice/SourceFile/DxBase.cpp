#include "stdafx.h"
#include "DxBase.h"

SceneManager& s_SceneManager = SceneManager::GetInstance();

DxBase::DxBase(HWND hWnd, HINSTANCE hInst)
{
	m_hWnd	= hWnd;
	m_hInst = hInst;
}


DxBase::~DxBase()
{
}

void DxBase::Init()
{
	ShowWindow(m_hWnd, SW_SHOW);
}

void DxBase::Update()
{
	UpdateWindow(m_hWnd);
}

void DxBase::Release()
{

}

Content::Content(HWND hWnd, HINSTANCE hInst) :DxBase(hWnd, hInst)
{

}

Content::~Content()
{
	
}

void Content::Init()
{
	ShowWindow(m_hWnd, SW_SHOW);

	m_pSceneFactory = make_shared<SceneFactory>();
	s_SceneManager.Init(m_pSceneFactory,m_hWnd);
}

void Content::Update()
{
	s_SceneManager.SceneUpdate();
	s_SceneManager.Render();
	UpdateWindow(m_hWnd);
}

void Content::Release() 
{

}