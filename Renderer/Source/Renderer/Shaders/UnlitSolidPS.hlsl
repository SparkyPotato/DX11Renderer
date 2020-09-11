cbuffer MaterialBuffer : register(b1)
{
	float4 materialColor;
	float ambientReflection;
	float specularReflection;
	float diffuseReflection;
	float shininess;
}

struct PSIn
{
	float4 worldPosition : POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
	float4 outPosition : SV_POSITION;
};

float4 main(PSIn input) : SV_TARGET
{
	return input.color * materialColor;
}