#pragma once

//�{�[��
class MyBone
{
public:
	XMMATRIX	m_Matrix;
	XMMATRIX	m_InitMat;	//�����p��
};

class RefBone
{
public:
	vector<float> Weight;
	vector<float> BoneId;
};

//�{�[���̏W��
class MySkelton
{
public:
	vector<MyBone> m_Bone;		//�{�[�����

	MySkelton();
	MySkelton(vector<MyBone> Bones);
	~MySkelton();
	int					GetBoneNum();
};

