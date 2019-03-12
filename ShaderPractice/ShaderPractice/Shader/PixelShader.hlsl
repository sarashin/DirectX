Texture2D g_Tex: register(t0);
SamplerState g_SampleLinear : register(s0);
Texture2D g_DepthTex:register(t1);

cbuffer global_1:register(b0)
{
	float4 g_Ambient = float4(0, 0, 0, 0);//�A���r�G���g��
	float3 g_Diffuse = float3(0, 0, 0);//�g�U����(�F�j
	float g_Alpha = 0.5;
};

//�o�͍\����
struct PS_OUTPUT {
	float4 Default;
};

//���͍\����
struct PS_INPUT
{
	float4 Pos		: SV_POSITION;
	float3 Normal	: NORMAL;
	float2 Tex		: TEXCOORD0;
	float4 Color	: TEXCOORD1;
	float  ShadowMap: TEXCOORD2;
	float3 View		: VEC;
};

float4 main(PS_INPUT In) : SV_TARGET
{
	if (dot(In.View,In.Normal) > 0.0) discard;

	float4 color = g_Tex.Sample(g_SampleLinear, In.Tex);
	color.w = g_Alpha;
	color.rgb *= g_Alpha;

	return color;
}