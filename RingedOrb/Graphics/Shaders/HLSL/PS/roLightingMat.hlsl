
#include "..\Headers\Lighting.hlsl"

struct VertexOut {
	float4 posH : SV_POSITION;
	float3 posW : POSITION;
	float3 normalW: NORMAL;
};

cbuffer MaterialCB : register(b1) {
	float4 gDiffuseAlbedo;
	float3 FresnelR0;
	float gRoughness;
}

cbuffer MainSceneCB : register(b2) {
	matrix gViewProj;
	matrix gView;
	matrix gProj;
	float3 gEyePosW;
	float pad;
	float4 gAmbient;
	Light gLight;
}

float4 PS(VertexOut pin) : SV_TARGET {

	pin.normalW = normalize(pin.normalW);
	float3 toEyeW = normalize(gEyePosW - pin.posW);

	float4 ambient = gAmbient * gDiffuseAlbedo;
	const float shininess = 1.0f - gRoughness;
	Material mat = {gDiffuseAlbedo, FresnelR0, shininess };
	float3 shadowFactor = 1.0f;
	float4 directLight = ComputeLighting(gLight, mat, pin.posW, pin.normalW, toEyeW, shadowFactor);

	float4 litColor = ambient + directLight;

	litColor.a = gDiffuseAlbedo.a;

	return litColor;
}