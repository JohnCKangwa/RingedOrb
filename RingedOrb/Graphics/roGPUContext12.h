#pragma once
#include "..\Core\pch.h"
#include "DX12IncludeFile.h"
#include "..\Core\roProfiler.h"
#include "roImageBuffer.h"
#include "roDepthBuffer.h"
#include "roGPUContextManager.h"
#include "roCommandQueue.h"
#include "..\Core\roWindow.h"

class roGPUContext12 : private RingedOrb::roGPUProfiler
{

	friend class roGPUContextManager;
public:
	roGPUContext12(std::string name);
	roGPUContext12(const roGPUContext12&) = delete;
	roGPUContext12 operator=(const roGPUContext12&) = delete;
	~roGPUContext12();

private:
	void Reset(std::string& name);
	void ExecuteCommands();
	void ExecuteOnWaitableQueue(int queueIdx, std::string workToWaitOn);
	uint64_t DiscardCommandAllocator();

public:

	RO_INLINE void BeginProfile(RingedOrb::roGPUProfileInfoType type) {
		roGPUProfiler::Begin(type, m_CommandList.Get());
	}

	RO_INLINE void EndProfile(RingedOrb::roGPUProfileInfoType type, int currFrameIdx) {
		roGPUProfiler::End(type, m_CommandList.Get(), currFrameIdx);
	}

	void CopyResource(roBaseGPUResource* src, roBaseGPUResource* dst);
	void CopyBufferRegion(roBaseGPUResource* src, UINT64 srcOffset, roBaseGPUResource* dst, UINT64 dstOffset, UINT64 numBytes);

	RO_INLINE void TransitionResource(roBaseGPUResource * resource, D3D12_RESOURCE_STATES new_state) {

		m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource->GetResource(), resource->m_resource_state, new_state));
		resource->m_resource_state = new_state;
	}

	RO_INLINE void SetDefaultViewport() {
		D3D12_VIEWPORT viewport;
		viewport.Width = static_cast<float>(roWindow::GetWidth());
		viewport.Height = static_cast<float>(roWindow::GetHeight());
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		m_CommandList->RSSetViewports(1, &viewport);
	}
	void SetCustomViewport(UINT width, UINT height, UINT topLeftX, UINT topLeftY);

	RO_INLINE void SetScissorRect() {
		D3D12_RECT rect{ 0,0,(LONG)roWindow::GetWidth(),(LONG)roWindow::GetHeight() };
		m_CommandList->RSSetScissorRects(1, &rect);
	}

	void SetViewports(D3D12_VIEWPORT* viewports, uint32_t numViewports);

	RO_INLINE void ClearRenderTarget(roImageBuffer* resource) {

		FLOAT ColorRGBA[4] = { .0f, .0f, .0f, 1.0 };

		m_CommandList->ClearRenderTargetView(resource->GetRTV(), ColorRGBA, 0, 0);
	}

	RO_INLINE void ClearDepthStencil(roDepthBuffer* resource) {
		m_CommandList->ClearDepthStencilView(resource->GetDSV(),
			D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0U,
			0U, 0U);
	}

	RO_INLINE void SetRenderTargets(roImageBuffer* imagebuffer, roDepthBuffer* depthbuffer) {
		if (depthbuffer == nullptr)
			m_CommandList->OMSetRenderTargets(1, &imagebuffer->GetRTV(), false, nullptr);
		else
			m_CommandList->OMSetRenderTargets(1, &imagebuffer->GetRTV(), false, &depthbuffer->GetDSV());
	}

	RO_INLINE void SetGraphicsRootSignature(ID3D12RootSignature* pRootSig) {
		m_CommandList->SetGraphicsRootSignature(pRootSig);
	}

	RO_INLINE void SetGraphicsPSO(ID3D12PipelineState* pPso) {
		m_CommandList->SetPipelineState(pPso);
	}

	RO_INLINE void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY& primitiveTopology) {
		m_CommandList->IASetPrimitiveTopology(primitiveTopology);
	}

	RO_INLINE void SetVertexBuffers(UINT StartSlot, UINT NumViews, const D3D12_VERTEX_BUFFER_VIEW* pVBViews) {
		m_CommandList->IASetVertexBuffers(StartSlot, NumViews, pVBViews);
	}

	RO_INLINE void SetIndexBuffer(D3D12_INDEX_BUFFER_VIEW* pIbv) {
		m_CommandList->IASetIndexBuffer(pIbv);
	}

	RO_INLINE void SetConstantBufferView(UINT rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS cbAddress) {
		m_CommandList->SetGraphicsRootConstantBufferView(rootParameterIndex, cbAddress);
	}

	RO_INLINE void Draw(int vertexCount, int startVertexLocation) {
		m_CommandList->DrawInstanced(vertexCount, 1, startVertexLocation, 0);
	}


	RO_INLINE void DrawIndexed(int indexCount, int startIndexLocation, int baseVertexLocation) {
		m_CommandList->DrawIndexedInstanced(indexCount, 1, startIndexLocation, baseVertexLocation, 0);
	}

	RO_INLINE void Finish(bool Isfinished, std::string newName = "", bool OnWaitableQueue = false, int queueIdx = -1, 
		std::string workToWaitOn = "") {		

		if (!OnWaitableQueue)
			ExecuteCommands();
		else
			ExecuteOnWaitableQueue(queueIdx, workToWaitOn);

		uint64_t fenceVal = DiscardCommandAllocator();

		RingedOrb::roGraphicsWorkInfo workInfo;
		workInfo.Name = m_Name;
		workInfo.FenceValue = fenceVal;

		RingedOrb::g_RecordedGraphicsWorkInformation.push_back(workInfo);
 
		if (Isfinished) {
			roGPUContextManager::DiscardGPUContext(this);
		}
		else {
			if (newName == "")
				THROW(L"graphics work name required on unreleased graphics work handle");

			Reset(newName);
		}
	}

	uint64_t Signal() const;
	ID3D12GraphicsCommandList* GetCommandList() const;
private:
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList = 0;
	ID3D12CommandAllocator* m_currCommandAllocator			= nullptr;
	std::string m_Name;
};

