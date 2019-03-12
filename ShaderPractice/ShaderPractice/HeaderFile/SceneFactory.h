#pragma once
#include"SceneBase.h"

//シーンの生成を行うクラス
class SceneFactory
{
public:
	SceneFactory();
	~SceneFactory();

	virtual bool GetFirstScene(unique_ptr<SceneBase>& pScene);
	virtual bool GetNextScene(unique_ptr<SceneBase>& pScene, DWORD NextScene);
	virtual void Release();
};

