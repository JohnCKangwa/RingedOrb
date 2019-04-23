#include "roGeometryGen.h"

std::vector<RingedOrb::Vertex> GeometryGen::GridLines(float width, float depth, unsigned int rowCount, unsigned int columnCount) {
	
	std::vector<RingedOrb::Vertex> vertices;

	float rowsize = width / (float)rowCount;
	float colsize = depth / (float)columnCount;

	{
		const float xStartOffset = ((rowsize * rowCount) / 2) * -1.0f;
		const float zConstantOffset = ((colsize * columnCount) / 2) * -1.0f;

		for (int rowIdx = 0; rowIdx <= rowCount; rowIdx++) {

			RingedOrb::Vertex vert;
			vert.position.x = (rowIdx * rowsize) + xStartOffset;
			vert.position.y = 0.0f;
			vert.position.z = zConstantOffset;

			vertices.push_back(vert);
			vert.position.z *= -1.0f;
			vertices.push_back(vert);
		}
	}

	{
		const float zStartOffset = ((rowsize * rowCount) / 2) * -1.0f;
		const float xConstantOffset = ((colsize * columnCount) / 2) * -1.0f;

		for (int colIdx = 0; colIdx <= rowCount; colIdx++) {
			RingedOrb::Vertex vert;
			vert.position.x = xConstantOffset;
			vert.position.y = 0.0f;
			vert.position.z = (colIdx * colsize) + zStartOffset;

			vertices.push_back(vert);
			vert.position.x *= -1.0f;
			vertices.push_back(vert);
			
		}
	}

	return vertices;
}
