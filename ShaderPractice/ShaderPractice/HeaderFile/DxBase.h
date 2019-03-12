#pragma once
#include"SceneManager.h"

class DxBase
{
protected:
	HWND		m_hWnd;
	HINSTANCE	m_hInst;

public:
	DxBase(HWND hWnd,HINSTANCE hInst);
	~DxBase();

	virtual void Init();
	virtual void Update();
	virtual void Release();
};

class Content : public DxBase
{
protected:
	shared_ptr<SceneFactory>	m_pSceneFactory;
public:
	Content(HWND hWnd, HINSTANCE hInst);
	~Content();

	void Init();
	void Update();
	void Release();
};