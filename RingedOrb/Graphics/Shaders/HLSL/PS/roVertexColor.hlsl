




struct VertexOut {
	float4 posH : SV_POSITION;
	float4 color: COLOR;
};

float4 PS(VertexOut vertexout) : SV_TARGET{

	return vertexout.color;
}