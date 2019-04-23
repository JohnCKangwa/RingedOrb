#pragma once
#include <DirectXMath.h>

using namespace DirectX;

struct roLight
{
	XMFLOAT3 Strength = { 0.5f, 0.5f, 0.5f };
	float FalloffStart = 1.0f;
	XMFLOAT3 Direction = { 0.0f, 0.0f, 1.0f };
	float FalloffEnd = 100.0f;
	XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };
	float SpotPower = 24.0f;
};