#include "stdafx.h"
#include "GameObject.h"

#define D3DX_PI ((FLOAT) 3.141592654f) 
#define D3DXToRadian( degree ) ((degree) * (D3DX_PI / 180.0f))
#define D3DXToDegree( radian ) ((radian) * (180.0f / D3DX_PI))

GameObject::GameObject()
{
	m_WorldMat.SetMat(XMMatrixIdentity());
	IsEnable = true;
}

GameObject::GameObject(Transform Trans)
{
	m_WorldMat = Trans;
	IsEnable = true;
}

GameObject::~GameObject()
{
	
}

void GameObject::SetChild(shared_ptr<GameObject> ChildObject)
{
	m_pChildObject = ChildObject;
}

void GameObject::SetParent(shared_ptr<GameObject> ParentObject)
{
	m_pParentObject = ParentObject;
}