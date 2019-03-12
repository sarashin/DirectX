#include"stdafx.h"
#include "Camera.h"

Camera::Camera()
{
	XMStoreFloat4x4(&m_ViewMat, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMat, XMMatrixIdentity());
	XMStoreFloat4x4(&m_WorldMat, XMMatrixIdentity());
	XMStoreFloat4x4(&m_LocalMat, XMMatrixIdentity());
}

Camera::~Camera()
{
}

//カメラの画角設定
void Camera::SetProjMat(float Rad, float Aspect, float ZNear, float ZFar)
{
	XMVECTOR InitVec=XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
	XMMATRIX View = XMMatrixIdentity();
	View *= XMMatrixRotationQuaternion(InitVec);
	View *= XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	XMStoreFloat4x4(&m_ViewMat, View);

	XMMATRIX Proj = XMMatrixIdentity();
	Proj = XMMatrixPerspectiveFovLH(Rad, Aspect, ZNear, ZFar);
	XMStoreFloat4x4(&m_ProjMat, Proj);
}

//ビュー行列作成
void Camera::SetViewMat(XMFLOAT3 Eye,XMFLOAT3 At,XMFLOAT3 Up)
{
	XMMATRIX ViewMat=XMMatrixLookAtLH(XMLoadFloat3(&Eye), XMLoadFloat3(&At), XMLoadFloat3(&Up));
	XMStoreFloat4x4(&m_ViewMat, ViewMat);
}

//ローカルのカメラ位置の移動
void Camera::TranslateL(XMVECTOR Trans)
{
	XMMATRIX Pos;
	Pos = XMLoadFloat4x4(&m_LocalMat)*XMMatrixTranslationFromVector(Trans);
	XMStoreFloat4x4(&m_LocalMat, Pos);
}

//ローカルのカメラ姿勢の変更
void Camera::SetTranslateL(XMVECTOR Trans)
{
	XMMATRIX Pos;
	Pos = XMMatrixTranslationFromVector(Trans);
	//Pos=XMMatrixInverse(Pos.r, Pos);
	XMStoreFloat4x4(&m_LocalMat, Pos);
}

//ローカルのカメラの回転
void Camera::RotationL(XMVECTOR Quatanion)
{
	XMMATRIX View = XMMatrixIdentity();
	XMMATRIX Quad = XMMatrixRotationQuaternion(Quatanion);
	Quad = XMMatrixInverse(Quad.r, Quad);
	View = XMLoadFloat4x4(&m_LocalMat) * Quad;
	XMStoreFloat4x4(&m_LocalMat, View);
}

//ワールドのカメラ位置の移動
void Camera::TranslateW(XMVECTOR Trans)
{
	XMMATRIX World = XMMatrixIdentity();
	XMFLOAT3 Pos;
	XMStoreFloat3(&Pos,Trans);
	World = XMLoadFloat4x4(&m_WorldMat)*XMMatrixTranslation(Pos.x, Pos.y, Pos.z);
	XMStoreFloat4x4(&m_WorldMat,World);
}
//ワールドのカメラ位置の変更
void Camera::SetTranslateW(XMVECTOR Trans)
{ 
	XMMATRIX World=XMLoadFloat4x4(&m_WorldMat);
	XMFLOAT3 Pos;
	XMStoreFloat3(&Pos, Trans);
	
	World._41 = Pos.x;
	World._42 = Pos.y;
	World._43 = Pos.z;
	XMMATRIX InvMat=XMLoadFloat4x4(&m_WorldMat);
	InvMat = XMMatrixInverse(World.r, World);
	XMStoreFloat4x4(&m_WorldMat, InvMat);


}
////ワールドのカメラの回転
//void Camera::RotationW(XMVECTOR Quatanion)
//{
//	WorldMat *= XMMatrixRotationQuaternion(Quatanion);
//}
////ワールドのカメラ回転の変更
//void Camera::SetRotationW(XMVECTOR Quatanion)
//{
//	WorldMat = XMMatrixRotationQuaternion(Quatanion);
//}

//ワールド座標変換したView行列取得
XMMATRIX Camera::GetViewMatW()
{
	XMMATRIX CameraW =XMLoadFloat4x4(&m_WorldMat) * XMLoadFloat4x4(&m_LocalMat) * XMLoadFloat4x4(&m_ViewMat);
	
	return CameraW;
}

//カメラのローカル座標取得
XMMATRIX Camera::GetViewMatL()
{
	return XMLoadFloat4x4(&m_ViewMat);
}

//カメラ座標取得
XMMATRIX Camera::GetWorldMat()
{
	XMMATRIX WorldMat = XMLoadFloat4x4(&m_WorldMat);
	XMMATRIX LocalMat = XMLoadFloat4x4(&m_LocalMat);
	return WorldMat*LocalMat;
}

//プロジェクション行列取得
XMMATRIX Camera::GetProjMat()
{
	return XMLoadFloat4x4(&m_ProjMat);
}

//カメラの座標取得
CXMVECTOR Camera::GetPos()
{
	XMMATRIX View = XMMatrixIdentity();
	View = XMLoadFloat4x4(&m_ViewMat);
	return XMMatrixDeterminant(View);
}

//カメラの前方ベクトル
CXMVECTOR Camera::GetFlont()
{
	XMMATRIX CameraW = XMLoadFloat4x4(&m_WorldMat) * XMLoadFloat4x4(&m_LocalMat) * XMLoadFloat4x4(&m_ViewMat);
	XMVECTOR Flont=XMLoadFloat3(&XMFLOAT3(CameraW._33-CameraW._23,0.0f,CameraW._31 - CameraW._21));

	return XMVector3Normalize(Flont);
}