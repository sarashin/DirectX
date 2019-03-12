#pragma once
#include"DxBase.h"

class WindowBase
{
	CREATESTRUCT m_WndStruct;
	HWND m_hWnd;
protected:
	virtual void PreCreate(CREATESTRUCT& pStruct, HWND Parent);
public:
	WindowBase();
	~WindowBase();
	bool Create(HINSTANCE hInst,const char* WndName,HWND Parent);
	static LRESULT CALLBACK LocalWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	HWND GetWindow();
};

class Window : public WindowBase
{

public:
	Window();
	~Window();

	void PreCreate(CREATESTRUCT& pStruct, HWND Parent)override;
	static LRESULT CALLBACK LocalWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
};

