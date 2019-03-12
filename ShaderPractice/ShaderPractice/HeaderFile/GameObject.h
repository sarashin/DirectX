#pragma once
#include"Transform.h"

//�Q�[���I�u�W�F�N�g
class GameObject
{
public:
	GameObject();
	GameObject(Transform Trans);
	~GameObject();

	Transform					m_WorldMat;			//���[���h�p��

	std::weak_ptr<GameObject>	m_pChildObject;		//�q�I�u�W�F�N�g�̃|�C���^
	std::weak_ptr<GameObject>	m_pParentObject;	//�e�I�u�W�F�N�g�̃|�C���^

	bool						IsEnable;			//�L�����ǂ���

	virtual void Release() = 0;

	void SetChild(shared_ptr<GameObject> ChildObject);
	void SetParent(shared_ptr<GameObject> ParentObject);
};
