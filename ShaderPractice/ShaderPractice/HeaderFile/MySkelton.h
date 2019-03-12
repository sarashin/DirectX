#pragma once

//ボーン
class MyBone
{
public:
	XMMATRIX	m_Matrix;
	XMMATRIX	m_InitMat;	//初期姿勢
};

class RefBone
{
public:
	vector<float> Weight;
	vector<float> BoneId;
};

//ボーンの集合
class MySkelton
{
public:
	vector<MyBone> m_Bone;		//ボーン情報

	MySkelton();
	MySkelton(vector<MyBone> Bones);
	~MySkelton();
	int					GetBoneNum();
};

