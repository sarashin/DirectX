#include"stdafx.h"
#include"Transform.h"



Transform::Transform()
{
	XMStoreFloat4x4(&TransformMatrix, XMMatrixIdentity());
}

Transform::Transform(CXMMATRIX Mat)
{
	XMStoreFloat4x4(&TransformMatrix, Mat);
}

Transform::~Transform()
{
}

void Transform::SetMat(CXMMATRIX Mat)
{
	XMStoreFloat4x4(&TransformMatrix,Mat);
}

XMMATRIX Transform::GetMat()
{
	return XMLoadFloat4x4(&TransformMatrix);
}

void Transform::SetPosition(XMVECTOR Pos)
{
	XMFLOAT3 Position;
	XMStoreFloat3(&Position, Pos);
	TransformMatrix._41 = Position.x;
	TransformMatrix._42 = Position.y;
	TransformMatrix._43 = Position.z;
}

void Transform::MovePosition(XMVECTOR Vec)
{
	XMMATRIX VecMat;
	VecMat = XMMatrixTranslationFromVector(Vec);
	XMMATRIX Mat = XMLoadFloat4x4(&TransformMatrix)*VecMat;
	XMStoreFloat4x4(&TransformMatrix, Mat);
}

XMVECTOR Transform::GetPosition()
{
	float x, y, z;
	XMFLOAT3 Pos;
	Pos.x = TransformMatrix._41;
	Pos.y = TransformMatrix._42;
	Pos.z = TransformMatrix._43;
	return XMLoadFloat3(&Pos);
}

void Transform::SetRotation(XMVECTOR Rotation)
{
	XMMATRIX RotationMat,TransMat;
	RotationMat = XMMatrixRotationRollPitchYawFromVector(Rotation);
	TransMat = XMMatrixTranslation(TransformMatrix._41, TransformMatrix._42, TransformMatrix._43);
	XMStoreFloat4x4(&TransformMatrix,RotationMat*TransMat);
}

void Transform::SetRotation(CXMMATRIX RotationMat)
{
	XMMATRIX RotateMat, TransMat;
	XMVECTOR Quat;
	Quat = XMQuaternionRotationMatrix(RotationMat);
	RotateMat = XMMatrixRotationRollPitchYawFromVector(Quat);
	TransMat = XMMatrixTranslation(TransformMatrix._41, TransformMatrix._42, TransformMatrix._43);
	XMStoreFloat4x4(&TransformMatrix, RotateMat*TransMat);
}

void Transform::RotationQuatanion(XMVECTOR Vec)
{
	XMMATRIX RotateMat = XMMatrixRotationQuaternion(Vec);
	XMMATRIX Mat = XMLoadFloat4x4(&TransformMatrix)*RotateMat;
	XMStoreFloat4x4(&TransformMatrix,Mat);
}

XMVECTOR Transform::GetRotationQuatanion()
{
	XMMATRIX Mat = XMLoadFloat4x4(&TransformMatrix);
	return XMQuaternionRotationMatrix(Mat);
}