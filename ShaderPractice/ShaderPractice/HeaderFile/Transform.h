#pragma once
//#include <xnamath.h>

class Transform
{
public:
	XMFLOAT4X4 TransformMatrix;

	Transform();
	Transform(CXMMATRIX Mat);
	~Transform();

	void SetMat(CXMMATRIX Matrix);
	XMMATRIX GetMat();
	void SetPosition(XMVECTOR Pos);
	void MovePosition(XMVECTOR Pos);
	XMVECTOR GetPosition();
	void SetRotation(XMVECTOR Rotation);
	void SetRotation(CXMMATRIX RotationMat);
	void RotationQuatanion(XMVECTOR Rot);
	XMVECTOR GetRotationQuatanion();
	/*void SetScale(float Scale);
	float GetScale();*/
};

