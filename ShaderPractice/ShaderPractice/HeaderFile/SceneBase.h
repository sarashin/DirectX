#pragma once
#include"D3Device.h"

enum SCENE_MESSAGE {
	SCENE_LOOP,
	SCENE_CHANGE,
	SCENE_RELOAD,
};

enum SCENE_NUM
{
	SCENE_TITLE,
	SCENE_GAME_FIRST,
	SCENE_GAME_SECOND,
	SCENE_GAME_THARD,
};

enum STATE_MSG
{
	LOOP,
	STOP,
	NEXT,
};

class SceneBase
{
protected:
public:
	SceneBase() {};
	~SceneBase() {};
	bool IsLoading;
	virtual void Load(shared_ptr<D3DRenderer> pRenderer) = 0;
	virtual void Init() = 0;
	virtual SCENE_MESSAGE Update(DWORD* pNextScene)=0;
	virtual void Render()=0;
	virtual void Release() = 0;
};