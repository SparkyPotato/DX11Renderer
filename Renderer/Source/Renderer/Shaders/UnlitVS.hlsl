cbuffer ObjectBuffer : register(b0)
{
	row_major float4x4 world;
	row_major float4x4 worldViewProjection;
}

struct VSIn
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
};

struct VSOut
{
	float4 worldPosition : POSITION;
	noperspective float3 normal : NORMAL;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
	float4 outPosition : SV_POSITION;
};

VSOut main(VSIn input)
{
	VSOut output;
	
	output.worldPosition = mul(float4(input.position, 1.f), world);
	output.normal = mul(float4(input.normal, 0.f), world).xyz;
	output.color = input.color;
	output.texcoord = input.texcoord;
	output.outPosition = mul(float4(input.position, 1.f), worldViewProjection);
	
	return output;
}