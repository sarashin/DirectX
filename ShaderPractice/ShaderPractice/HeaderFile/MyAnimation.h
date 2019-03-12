#pragma once
#include"Mesh.h"
#include"MySkelton.h"

using namespace fbxsdk;
using namespace Microsoft::WRL;
using namespace std;


//アニメーション
class MySkinAnimation
{
public:
	MySkinAnimation();
	~MySkinAnimation();

	shared_ptr<MySkelton>	m_Skelton;		//ボーン情報
	vector<XMMATRIX>		m_AnimMatrix;	//各フレームの姿勢(主ボーン)
	vector<vector<XMMATRIX>> m_AnimBoneMatrix;//各フレームの姿勢(その他ボーン)

	int Start;						//開始フレーム
	int Stop;						//終了フレーム

	string Name;					//名前

	int GetBoneNum();
	void Release();
};

//アニメーションコンテナ
class MyAnimationContainer
{
	vector<MySkinAnimation> m_Animation;//アニメーションコンテナ
	map<string,int>			m_AnimIndex;	//インデックス

	shared_ptr<MyMeshData> m_Mesh;		//メッシュへのポインタ

	int					NowAnimNum;		//現アニメーションのインデックスナンバー
	string				NowAnimName;	//名前
public:
	MyAnimationContainer();
	MyAnimationContainer(vector<MySkinAnimation> Animation);

	ID3D11Buffer*		m_BoneBuffer;	//シェーダへ渡すボーン配列

	void SetAnim(MySkinAnimation* Anim,string Name);
	void GetAnimMat(XMMATRIX* OptMat);
};