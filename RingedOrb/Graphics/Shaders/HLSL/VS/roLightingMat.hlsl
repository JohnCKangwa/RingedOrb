#include "..\Headers\Lighting.hlsl"

struct VertexIn {
	float3 posL : POSITION;
	float3 normalL: NORMAL;
};

struct VertexOut {
	float4 posH : SV_POSITION;
	float3 posW : POSITION;
	float3 normalW: NORMAL;
};

cbuffer ObjectCB : register(b0) {
	matrix gWorld;
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

VertexOut VS(VertexIn vertexin) {

	VertexOut vout;

	float4 posW = float4(vertexin.posL, 1.0f);
	posW = mul(posW, gWorld);
	vout.posW = posW.xyz;

	vout.normalW = mul(vertexin.normalL, (float3x3)gWorld);
	vout.posH = mul(posW, gViewProj);

	return vout;
}
