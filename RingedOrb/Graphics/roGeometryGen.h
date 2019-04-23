#pragma once
#include "..\Core\pch.h"
#include "..\Graphics\roVertex.h"

struct GeometryGen {

	static std::vector<RingedOrb::Vertex> GridLines(float width, float height, unsigned int rows, unsigned int columns);
	
};