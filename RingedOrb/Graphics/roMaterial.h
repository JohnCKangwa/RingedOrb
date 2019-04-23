#pragma once
#include "..\Core\pch.h"
#include <DirectXMath.h>

struct roMaterial {
	DirectX::XMFLOAT4 DiffuseAlbedo = { 0.10f, 0.10f, 0.70f, 1.0f };
	DirectX::XMFLOAT3 FresnelR0 = { 0.1f,0.11f,0.1f };
	float Shininess = 0.50;
};