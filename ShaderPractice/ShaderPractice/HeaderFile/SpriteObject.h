#pragma once
#include "GameObject.h"
#include "Sprite.h"
#include "D3Device.h"

class SpriteObject :public GameObject
{
	shared_ptr<Sprite>	m_pSprite;
public:
	bool				IsAppear;

	//シェーダへ渡すバッファ
	ComPtr<ID3D11Buffer>			m_pMatrixBuffer;

	SpriteObject();
	SpriteObject(Sprite* pSprite);
	~SpriteObject();

	void SetMeshObject(shared_ptr<Sprite>& pSprite);
	Sprite* GetMeshObject();

	void Create(const char* TexPath);
	void Update();
	void Draw(Camera Cam,ComPtr<ID3D11DeviceContext> pDevContext);
	void CopyObject(SpriteObject** ppObject);
	void Release();
};

