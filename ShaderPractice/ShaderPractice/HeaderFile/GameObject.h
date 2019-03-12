#pragma once
#include"Transform.h"

//ゲームオブジェクト
class GameObject
{
public:
	GameObject();
	GameObject(Transform Trans);
	~GameObject();

	Transform					m_WorldMat;			//ワールド姿勢

	std::weak_ptr<GameObject>	m_pChildObject;		//子オブジェクトのポインタ
	std::weak_ptr<GameObject>	m_pParentObject;	//親オブジェクトのポインタ

	bool						IsEnable;			//有効かどうか

	virtual void Release() = 0;

	void SetChild(shared_ptr<GameObject> ChildObject);
	void SetParent(shared_ptr<GameObject> ParentObject);
};
