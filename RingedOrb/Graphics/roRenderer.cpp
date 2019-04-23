#include "roRenderer.h"
#include "roGraphicsResources.h"
#include "roRenderableEntity.h"
#include "roConstantBuffer.h"
#include "roConstantRingBuffer.h"
#include "roRenderableEntity.h"
#include "..\Graphics\roGPUContext12.h"
#include "roModelLoader.h"
#include "..\Core\roFileSystem.h"
#include "roLighting.h"
#include "roVertex.h"

CB256_ALIGN ObjectCB {
	DirectX::XMFLOAT4X4 world;
};

CB256_ALIGN MaterialCB {
	DirectX::XMFLOAT4 diffuseAlbedo;
	DirectX::XMFLOAT3 FresnelR0;
	float Shininess;
};

CB256_ALIGN MainCB {
	DirectX::XMFLOAT4X4 viewProj;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 proj;
	DirectX::XMFLOAT3 eyePos;
	float pad;
	DirectX::XMFLOAT4 AmbientLight;
	roLight light;
};

void roGraphicsRenderer::Initialize(roConstantRingBuffer* constantRingBuffer) {
	using namespace DirectX;
	
	sm_ConstantRingBuffer = constantRingBuffer;
	sm_IsCreated = true;
	
	sm_Cameras.push_back(roViewProjection());

	sm_WireframeState = false;

}

void roGraphicsRenderer::CopyRenderableEntities(std::vector<roRenderableEntity*>& enties) {
	sm_RenderableEntities.rEntities = enties;
}

void roGraphicsRenderer::CopyRenderableEntitiesOpaque(std::vector<roRenderableEntity*> entities[RET_OPAQUE_COUNT]) {
	for( int opqArrIdx = 0; opqArrIdx < RET_OPAQUE_COUNT; opqArrIdx++)
		sm_RenderableEntities.rOpaqueEntities[opqArrIdx] = entities[opqArrIdx];
}

void roGraphicsRenderer::SetWireframeState(bool state) {
	sm_WireframeState = state; 
}

bool roGraphicsRenderer::GetWireframeState() {
	return sm_WireframeState;
}

void roGraphicsRenderer::UpdateMainPassConstant() {
	using namespace DirectX;

	MainCB mainPassConstantBuffer;

	XMMATRIX projMatrix;
	XMMATRIX viewMatrix;
	XMMATRIX viewProjMatrix;

	projMatrix = XMLoadFloat4x4(&sm_Cameras[0].GetProjection());
	viewMatrix = XMLoadFloat4x4(&sm_Cameras[0].GetView());
	viewProjMatrix = XMLoadFloat4x4(&sm_Cameras[0].GetViewProjection());

	XMStoreFloat4x4(&mainPassConstantBuffer.proj, XMMatrixTranspose(projMatrix));
	XMStoreFloat4x4(&mainPassConstantBuffer.view, XMMatrixTranspose(viewMatrix));
	XMStoreFloat4x4(&mainPassConstantBuffer.viewProj, XMMatrixTranspose(viewProjMatrix));
	mainPassConstantBuffer.eyePos = DirectX::XMFLOAT3(0.0f, 0.0f, -20);
	mainPassConstantBuffer.AmbientLight = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);

	sm_MainPassRingOffset = sm_ConstantRingBuffer->UploadConstantData(&mainPassConstantBuffer, sizeof(mainPassConstantBuffer));

}

void roGraphicsRenderer::UpdateRenderableEntities() {
	using namespace DirectX;

  	for (auto &RenderableE : sm_RenderableEntities.rEntities) {
		ObjectCB objCB;

		XMStoreFloat4x4(&objCB.world, XMMatrixTranspose(XMLoadFloat4x4(&RenderableE->WorldTransform)));
		RenderableE->ObjectConstantBufferRingOffset = sm_ConstantRingBuffer->UploadConstantData(&objCB, sizeof(ObjectCB));

		if (RenderableE->Material) {
			MaterialCB matCB;

			matCB.diffuseAlbedo = RenderableE->Material->DiffuseAlbedo;
			matCB.FresnelR0 = RenderableE->Material->FresnelR0;
			matCB.Shininess = RenderableE->Material->Shininess;

			RenderableE->MaterialConstantBufferRingOffset = sm_ConstantRingBuffer->UploadConstantData(&matCB, sizeof(MaterialCB));
		}
	}
 }

void roGraphicsRenderer::Draw(roGPUContext12* mainGpuContext) {

	if (sm_WireframeState == true) {
		DrawWireframe(mainGpuContext);
		return;
	}
	//draw objects with main gpuContextThread
	DrawOpaque(mainGpuContext);
	//draw rest on workers
}

void roGraphicsRenderer::DrawOpaque(roGPUContext12* gpuContext) {
	DrawOpaqueVertColor(gpuContext);
	DrawOpaqueLightMat(gpuContext);
	DrawOpaqueLines(gpuContext);
}

void roGraphicsRenderer::DrawOpaqueVertColor(roGPUContext12 * gpuContext) {

	auto rootSig = roGraphicsResourceGet::RootSignature("RootSig0");
	auto pso = roGraphicsResourceGet::PSO("VertexColorPSO");

	gpuContext->SetGraphicsRootSignature(rootSig);
	gpuContext->SetGraphicsPSO(pso);

	D3D12_GPU_VIRTUAL_ADDRESS mainPassCBAddress = sm_ConstantRingBuffer->GetAddressAtOffset(sm_MainPassRingOffset);
	gpuContext->SetConstantBufferView(2, mainPassCBAddress);

	for (auto &RenderableE : sm_RenderableEntities.rOpaqueEntities[RET_OPAQUE_VERT_COLOR]) {
		bool isIndexEnabled = RenderableE->Mesh->IndicesState();
		
		if (isIndexEnabled) {
			auto ibv = RenderableE->Mesh->GetIndexBufferView();
			gpuContext->SetIndexBuffer(&ibv);
		}

		auto vbv = RenderableE->Mesh->GetVertexBufferView();

		gpuContext->SetPrimitiveTopology(RenderableE->PrimitiveTopologyType);
		gpuContext->SetVertexBuffers(0, 1, &vbv);

		for (auto& drawArgs : RenderableE->Mesh->MeshInfo) {
			if (!isIndexEnabled)
				gpuContext->Draw(drawArgs.VertexCount, drawArgs.StartIndexLocation);
			else
				gpuContext->DrawIndexed(drawArgs.IndexCount, drawArgs.StartIndexLocation, drawArgs.BaseVertexLocation);
		} 
	}
}

void roGraphicsRenderer::DrawOpaqueLightMat(roGPUContext12 * gpuContext) {

	auto rootSig = roGraphicsResourceGet::RootSignature("RootSig0");
	auto pso = roGraphicsResourceGet::PSO("OpaqueLightingMatPSO");

	gpuContext->SetGraphicsRootSignature(rootSig);
	gpuContext->SetGraphicsPSO(pso);

	D3D12_GPU_VIRTUAL_ADDRESS mainPassCBAddress = sm_ConstantRingBuffer->GetAddressAtOffset(sm_MainPassRingOffset);
	gpuContext->SetConstantBufferView(2, mainPassCBAddress);

	for (auto &RenderableE : sm_RenderableEntities.rOpaqueEntities[RET_OPAQUE_LIGHT_MAT]) {
		bool isIndexEnabled = RenderableE->Mesh->IndicesState();

		if (isIndexEnabled) {
			auto ibv = RenderableE->Mesh->GetIndexBufferView();
			gpuContext->SetIndexBuffer(&ibv);
		}

		auto vbv = RenderableE->Mesh->GetVertexBufferView();

		gpuContext->SetPrimitiveTopology(RenderableE->PrimitiveTopologyType);
		gpuContext->SetVertexBuffers(0, 1, &vbv);
		D3D12_GPU_VIRTUAL_ADDRESS cbObjectAddress = sm_ConstantRingBuffer->GetAddressAtOffset(RenderableE->ObjectConstantBufferRingOffset);
		gpuContext->SetConstantBufferView(0, cbObjectAddress);

		D3D12_GPU_VIRTUAL_ADDRESS cbMaterialAddress = sm_ConstantRingBuffer->GetAddressAtOffset(RenderableE->MaterialConstantBufferRingOffset);
		gpuContext->SetConstantBufferView(1, cbMaterialAddress);

		for (auto& drawArgs : RenderableE->Mesh->MeshInfo) {
			if (!isIndexEnabled)
				gpuContext->Draw(drawArgs.VertexCount, drawArgs.StartIndexLocation);
			else
				gpuContext->DrawIndexed(drawArgs.IndexCount, drawArgs.StartIndexLocation, drawArgs.BaseVertexLocation);
		}
	}
}

void roGraphicsRenderer::DrawOpaqueLines(roGPUContext12* gpuContext) {

	auto rootSig = roGraphicsResourceGet::RootSignature("RootSig0");
	auto pso = roGraphicsResourceGet::PSO("LinesPSO");

	gpuContext->SetGraphicsRootSignature(rootSig);
	gpuContext->SetGraphicsPSO(pso);

	D3D12_GPU_VIRTUAL_ADDRESS mainPassCBAddress = sm_ConstantRingBuffer->GetAddressAtOffset(sm_MainPassRingOffset);
	gpuContext->SetConstantBufferView(2, mainPassCBAddress);

	for (auto &RenderableE : sm_RenderableEntities.rOpaqueEntities[RET_OPAQUE_LINES]) {

		bool isIndexEnabled = RenderableE->Mesh->IndicesState();

		if (isIndexEnabled) {
			auto ibv = RenderableE->Mesh->GetIndexBufferView();
			gpuContext->SetIndexBuffer(&ibv);
		}

		auto vbv = RenderableE->Mesh->GetVertexBufferView();

		gpuContext->SetPrimitiveTopology(RenderableE->PrimitiveTopologyType);
		gpuContext->SetVertexBuffers(0, 1, &vbv);
		D3D12_GPU_VIRTUAL_ADDRESS cbAddress = sm_ConstantRingBuffer->GetAddressAtOffset(RenderableE->ObjectConstantBufferRingOffset);
		gpuContext->SetConstantBufferView(0, cbAddress);

		for (auto& drawArgs : RenderableE->Mesh->MeshInfo) {
			if (!isIndexEnabled)
				gpuContext->Draw(drawArgs.VertexCount, drawArgs.StartIndexLocation);
			else
				gpuContext->DrawIndexed(drawArgs.IndexCount, drawArgs.StartIndexLocation, drawArgs.BaseVertexLocation);
		}
	}
}

void roGraphicsRenderer::DrawWireframe(roGPUContext12* gpuContext) {

	auto rootSig = roGraphicsResourceGet::RootSignature("RootSig0");
	auto pso = roGraphicsResourceGet::PSO("WireframePSO");
	
	gpuContext->SetGraphicsRootSignature(rootSig);
	gpuContext->SetGraphicsPSO(pso);

	D3D12_GPU_VIRTUAL_ADDRESS mainPassCBAddress = sm_ConstantRingBuffer->GetAddressAtOffset(sm_MainPassRingOffset);
	gpuContext->SetConstantBufferView(2, mainPassCBAddress);

	for (auto &RenderableE : sm_RenderableEntities.rEntities) {

		bool isIndexEnabled = RenderableE->Mesh->IndicesState();

		if (isIndexEnabled) {
			auto ibv = RenderableE->Mesh->GetIndexBufferView();
			gpuContext->SetIndexBuffer(&ibv);
		}

		auto vbv = RenderableE->Mesh->GetVertexBufferView();

		gpuContext->SetPrimitiveTopology(RenderableE->PrimitiveTopologyType);
		gpuContext->SetVertexBuffers(0, 1, &vbv);
		D3D12_GPU_VIRTUAL_ADDRESS cbAddress = sm_ConstantRingBuffer->GetAddressAtOffset(RenderableE->ObjectConstantBufferRingOffset);
		gpuContext->SetConstantBufferView(0, cbAddress);

		for (auto& drawArgs : RenderableE->Mesh->MeshInfo) {
			if (!isIndexEnabled)
				gpuContext->Draw(drawArgs.VertexCount, drawArgs.StartIndexLocation);
			else
				gpuContext->DrawIndexed(drawArgs.IndexCount, drawArgs.StartIndexLocation,drawArgs.BaseVertexLocation);
		}
	}
}

void roGraphicsRenderer::Shutdown() {

	sm_IsCreated = false;
}

bool roGraphicsRenderer::sm_IsCreated = false;
bool roGraphicsRenderer::sm_WireframeState = false;
roConstantRingBuffer* roGraphicsRenderer::sm_ConstantRingBuffer;
std::vector<roViewProjection> roGraphicsRenderer::sm_Cameras;
UINT64 roGraphicsRenderer::sm_MainPassRingOffset;

roGraphicsRenderer::RenderableEntities roGraphicsRenderer::sm_RenderableEntities;

