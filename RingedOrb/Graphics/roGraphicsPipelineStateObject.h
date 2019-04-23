#pragma once
#include "DX12IncludeFile.h"
#include "..\Core\pch.h"

enum roPSOType {
	enumPsoWireless = 0,
	enumPsoOpaqueVertexColors = 1,
	enumPsoOpaqueLightMat = 2,
	enumPsoOpaqueLines = 3,
	enumPsoCount = 4
};

class roGraphicsPipelineStateObjects {
	friend struct roGraphicsResourceLoader;
private:
	static void Initialize();
	static void CreatePSOs(roPSOType psotype);
private:
	static void CreatePSOFromDesc(std::string name, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
	static void CreateWireframePSO();
	static void CreateOpaqueVertexColorsPSO();
	static void CreateOpaqueLightMatPSO();
	static void CreateOpaqueLightMatTexPSO();
	static void CreateOpaqueLinesPSO();
	//static void CreateMarkMirrorPSO();
	//static void CreateReflectionsPSO();

private:
	static void DefaultDesc();
	static void WireframeDesc();
	static void TransparentDesc();

private:
	static D3D12_GRAPHICS_PIPELINE_STATE_DESC sm_DefaultDesc;
	static D3D12_GRAPHICS_PIPELINE_STATE_DESC sm_WireframeDesc;
	static D3D12_GRAPHICS_PIPELINE_STATE_DESC sm_TransparentDesc;

	static D3D12_INPUT_ELEMENT_DESC sm_InputElemDescPos;
	static D3D12_INPUT_ELEMENT_DESC sm_InputElemDescPosColor[2];
	static D3D12_INPUT_ELEMENT_DESC sm_InputElemDescPosNormal[2];
	static D3D12_INPUT_ELEMENT_DESC sm_InputElemDescPosNormalTex[3];
};