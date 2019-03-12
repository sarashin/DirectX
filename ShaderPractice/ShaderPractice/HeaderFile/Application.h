#pragma once
#include"Window.h"

//アプリケーションクラス
class AppBase
{
private:
protected:
	HINSTANCE m_hInstance;
	WNDCLASSEX m_WndClass;

	vector<shared_ptr<Window>> m_pWndAry;
	virtual HRESULT RegistWndClass();
	virtual HRESULT CreateMainWindow();
	virtual bool	Init()=0;
	virtual void	Update();
	virtual void	Release()=0;
public:
	AppBase();
	virtual ~AppBase();

	HRESULT Run(HINSTANCE hInst);

	static LRESULT CALLBACK GrobalWndProc( HWND hWnd,
									UINT Msg,
									WPARAM wParam,
									LPARAM lParam
									);

	HINSTANCE GetHInstance();
	void GetWindowClass(unique_ptr<WNDCLASSEX> pWndClass);
	vector<shared_ptr<Window>>* GetWndPtr();
};

class Application:public AppBase
{
public:
	unique_ptr<DxBase> m_DxBase;

	HRESULT RegistWndClass()override;
	HRESULT CreateMainWindow()override;
	bool	Init();
	void	Update();
	void	Release();
	Application();
	~Application();
};
