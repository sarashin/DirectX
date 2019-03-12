#pragma once
#include"Mesh.h"
#include"MySkelton.h"

using namespace fbxsdk;
using namespace Microsoft::WRL;
using namespace std;


//�A�j���[�V����
class MySkinAnimation
{
public:
	MySkinAnimation();
	~MySkinAnimation();

	shared_ptr<MySkelton>	m_Skelton;		//�{�[�����
	vector<XMMATRIX>		m_AnimMatrix;	//�e�t���[���̎p��(��{�[��)
	vector<vector<XMMATRIX>> m_AnimBoneMatrix;//�e�t���[���̎p��(���̑��{�[��)

	int Start;						//�J�n�t���[��
	int Stop;						//�I���t���[��

	string Name;					//���O

	int GetBoneNum();
	void Release();
};

//�A�j���[�V�����R���e�i
class MyAnimationContainer
{
	vector<MySkinAnimation> m_Animation;//�A�j���[�V�����R���e�i
	map<string,int>			m_AnimIndex;	//�C���f�b�N�X

	shared_ptr<MyMeshData> m_Mesh;		//���b�V���ւ̃|�C���^

	int					NowAnimNum;		//���A�j���[�V�����̃C���f�b�N�X�i���o�[
	string				NowAnimName;	//���O
public:
	MyAnimationContainer();
	MyAnimationContainer(vector<MySkinAnimation> Animation);

	ID3D11Buffer*		m_BoneBuffer;	//�V�F�[�_�֓n���{�[���z��

	void SetAnim(MySkinAnimation* Anim,string Name);
	void GetAnimMat(XMMATRIX* OptMat);
};