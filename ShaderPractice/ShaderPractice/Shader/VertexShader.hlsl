

cbuffer global_0:register(b0)
{
	matrix g_W;			//���[���h�s��
	matrix g_View;		//�r���[�s��
	matrix g_Proj;		//�v���W�F�N�V�����s��
	float4 g_vLight;	//���C�g����
};

cbuffer global_1:register(b1)
{
	float4 g_Ambient = float4(0, 0, 0, 1);//�A���r�G���g��
	float4 g_Diffuse = float4(0, 0, 0, 1);//�g�U����(�F�j
	float g_Alpha = 1.0;
};

cbuffer global_2:register(b2)
{
	matrix g_BoneMat[4]; //�{�[���p���s��
};
                
//���͍\����
struct VS_IN {                     
    float4	Position: POSITION;      
	float3	Normal	: NORMAL;
	float2	Tex		: TEXCOORD0;
};                                 
                                   
//�o�[�e�b�N�X�o�b�t�@�[�o�͍\����
struct VS_OUTPUT
{
	float4 Position	: SV_POSITION;
	float3 Normal	: Normal;
	float2 Tex		: TEXCOORD0;
	float4 Color	: TEXCOORD1;
	float ShadowMap : TEXCOORD2;
	float3 View		: VEC;
};
                                   
VS_OUTPUT main( VS_IN In )
{          
    VS_OUTPUT Out = (VS_OUTPUT)0; 
	/*float w[3] = (float[3])In.Blend;  
	float4x4 comb = (float4x4)0;      
	for ( int i = 0; i < 3; i++ )     
		comb += g_BoneMat[In.Idx[i]] * w[i];
  
	comb += g_BoneMat[In.Idx[3]] * (1.0f - w[0] - w[1] - w[2]);*/

	float4 Pos = mul(In.Position, g_W);
    Pos = mul( Pos, g_View );   
    Out.Position = mul( Pos, g_Proj );   

	Pos = mul(In.Position, g_W);
	float4 LightPos = Pos * normalize(g_vLight) * -1;
	Out.ShadowMap = length(LightPos) / length(g_vLight);

	//�@�������[���h��Ԃ�
	Out.Normal = mul(In.Normal, g_W);
	float3 N = normalize(Out.Normal);
	Out.View = normalize(float3(g_View._13, 0.0f, g_View._33));
	float Shadow = saturate(dot(N, g_vLight*-1));
	Shadow = Shadow * 0.5f + 0.5f;
	Out.Color = float4(Shadow, Shadow, Shadow, 1.0);
	/*float3 LightDir = normalize(float3(g_vLightView._13, g_vLightView._23, g_vLightView._33));
	Out.Color = float4(0.0, 0.6, 1.0, 1.0) * (0.3 + dot(N, -LightDir)*(1 - 0.3f));*/
	////���C�g����
	//Out.Light = g_vLightDir;
	////�����x�N�g��
	//float3 PosWorld = normalize(Out.Pos);
	//Out.EyeVector = g_vEye - PosWorld;

	/*float3 Normal = normalize(Out.Normal);
	float3 LightDir = normalize(g_vLightDir);
	float3 ViewDir = normalize(g_vEye - PosWorld);
	float4 NL = saturate(dot(Normal, LightDir));*/

	//float3 Reflect = normalize(2 * NL * Normal - LightDir);
	//float4 Specular = pow(saturate(dot(Reflect, ViewDir)), 4);
	//Out.Color = g_Diffuse * NL + g_Ambient;

	//float4 Shadow=

	//�e�N�X�`���[���W
	Out.Tex = In.Tex;

    return Out;                       
}                                     