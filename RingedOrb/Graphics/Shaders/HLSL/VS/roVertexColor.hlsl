#include "..\Headers\Lighting.hlsl"

struct VertexIn {
	float3 posL : POSITION;
	float4 color: COLOR;
};

struct VertexOut {
	float4 posH : SV_POSITION;
	float4 color: COLOR;
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
	vout.posH = mul(posW, gViewProj);

	vout.color = vertexin.color;

	return vout;
}
