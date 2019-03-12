#pragma once
#define D3DX_PI ((FLOAT) 3.141592654f) 
#define D3DXToRadian( degree ) ((degree) * (D3DX_PI / 180.0f))

class Camera
{
	XMFLOAT4X4 m_WorldMat;
	XMFLOAT4X4 m_LocalMat;
	XMFLOAT4X4 m_ViewMat;
	XMFLOAT4X4 m_ProjMat;
public:
	
	Camera();
	~Camera();

	void SetProjMat(float Rad,float Aspect,float ZNear,float ZFar);
	void SetViewMat(XMFLOAT3 Eye,XMFLOAT3 At,XMFLOAT3 Up);

	void TranslateL(XMVECTOR Trans);
	void SetTranslateL(XMVECTOR Trans);
	void RotationL(XMVECTOR Quatanion);
	void SetRotationL(XMVECTOR Quatanion);
	void TranslateW(XMVECTOR Trans);
	void SetTranslateW(XMVECTOR Trans);
	void RotationW(XMVECTOR Quatanion);
	void SetRotationW(XMVECTOR Quatanion);
	CXMVECTOR GetPos();
	CXMVECTOR GetFlont();

	XMMATRIX GetWorldMat();
	XMMATRIX GetViewMatL();
	XMMATRIX GetViewMatW();
	XMMATRIX GetProjMat();
	void Zoom(float Mag);
};

