


struct VertexOut {
	float4 posH : SV_POSITION;
};


//wireframe material for this object
cbuffer MaterialCB : register(b1) {
	float4 gMatColor;
}

float4 PS(VertexOut vertexout) : SV_TARGET{
	return float4(1.0f,1.0f,1.0f,1.0f);
}