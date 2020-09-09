cbuffer LightBuffer : register(b0)
{
	float4 lightPosition;
	float3 ambientColor;
	float ambientIntensity;
	float3 specularColor;
	float specularIntensity;
	float3 diffuseColor;
	float diffuseIntensity;
	float attConstant;
	float attLinear;
	float attQuadratic;
}

cbuffer MaterialBuffer : register(b1)
{
	float ambientReflection;
	float specularReflection;
	float diffuseReflection;
	float shininess;
}

cbuffer CameraBuffer : register(b2)
{
	float4 cameraPosition;
}

struct PSIn
{
	float4 worldPosition : POSITION;
	noperspective float3 normal : NORMAL;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
	float4 outPosition : SV_POSITION;
};

float4 main(PSIn input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	
	float3 ambientValue = ambientColor * ambientIntensity * ambientReflection;
	
	float3 lightDirection = lightPosition.xyz - input.worldPosition.xyz;
	float distance = length(lightDirection);
	lightDirection /= distance;
	distance *= distance;
	
	float attenuation = 1.0f / (attConstant + attLinear * distance + attQuadratic * (distance * distance));
	
	float normalDot = max(dot(lightDirection, input.normal), 0.f);
	float intensity = saturate(normalDot);
	
	float3 diffuseValue = intensity * diffuseColor * diffuseIntensity * diffuseReflection * attenuation;
	
	if (length(diffuseValue) != 0)
	{
		float3 reflection = reflect(lightDirection, input.normal);
		float3 view = normalize(input.worldPosition.xyz - cameraPosition.xyz);
		float viewDot = max(dot(reflection, view), 0.f);
		intensity = pow(saturate(viewDot), shininess);
	
		float3 specularValue = intensity * specularReflection * specularColor * specularIntensity * attenuation;
		
		return float4(saturate(ambientValue + diffuseValue + specularValue) * input.color.rgb, input.color.a);
	}
	
	return float4(saturate(ambientValue) * input.color.rgb, input.color.a);

}