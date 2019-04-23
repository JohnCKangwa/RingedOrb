#include "roViewProjection.h"




roViewProjection::roViewProjection() {
	Initialize();
}


void roViewProjection::Initialize() {
	using namespace DirectX;
	SetScreenProjection(0.25 * XM_PI, 1.333f, 1.0f, 1000.0f);
	CalculateExperimentalViewMatrix();
}

void roViewProjection::SetScreenProjection(float verticalFieldOfView, float aspectRatio, float nearPlane, float farPlane) {
	using namespace DirectX;
	XMStoreFloat4x4(&m_ScreenProjection, XMMatrixPerspectiveFovLH(verticalFieldOfView, aspectRatio, nearPlane, farPlane));
}

void roViewProjection::CalculateExperimentalViewMatrix() {
	using namespace DirectX;

	auto viewMatrix = XMMatrixLookAtLH(XMVectorSet(-6.0, 8.0f, -10.0f, 1.0f), XMVectorSet(0.0, 0.0f, 0.0f, 0.0f), 
		XMVectorSet(0.0, 1.0f, 0.0f, 0.0f));

	XMStoreFloat4x4(&m_View, viewMatrix);
}

DirectX::XMFLOAT4X4 roViewProjection::GetView() { 
	return m_View; 
}

DirectX::XMFLOAT4X4 roViewProjection::GetProjection() {
	return m_ScreenProjection; 
}

DirectX::XMFLOAT4X4 roViewProjection::GetViewProjection() {
	using namespace DirectX;

	XMFLOAT4X4 viewProj;
	XMStoreFloat4x4(&viewProj, XMLoadFloat4x4(&m_View) * XMLoadFloat4x4(&m_ScreenProjection));

	return viewProj;
}
