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
    return input.color;
}