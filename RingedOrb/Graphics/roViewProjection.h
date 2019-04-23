#pragma once
#include <DirectXMath.h>

class roViewProjection {
public:
	roViewProjection();
private:
	void Initialize();
	void SetScreenProjection(float verticalFieldOfView, float aspectRatio, float nearPlane, float farPlane);

	void CalculateExperimentalViewMatrix();
public:
	DirectX::XMFLOAT4X4 GetView();
	DirectX::XMFLOAT4X4 GetProjection();
	DirectX::XMFLOAT4X4 GetViewProjection();

private:
	DirectX::XMFLOAT4X4 m_View;
	DirectX::XMFLOAT4X4 m_ScreenProjection;
};