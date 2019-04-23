#include "roGraphicsPipelineStateObject.h"
#include "roRootGraphics.h"
#include "..\Core\roErrorHandling.h"
#include "roGraphicsResources.h"

void roGraphicsPipelineStateObjects::DefaultDesc()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.RasterizerState.FillMode				= D3D12_FILL_MODE_SOLID;
	psoDesc.RasterizerState.CullMode				= D3D12_CULL_MODE_BACK;
	psoDesc.RasterizerState.FrontCounterClockwise	= FALSE;
	psoDesc.RasterizerState.DepthBias				= D3D12_DEFAULT_DEPTH_BIAS;
	psoDesc.RasterizerState.SlopeScaledDepthBias	= D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	psoDesc.RasterizerState.DepthClipEnable			= TRUE;
	psoDesc.RasterizerState.MultisampleEnable		= FALSE;
	psoDesc.RasterizerState.AntialiasedLineEnable	= FALSE;
	psoDesc.RasterizerState.ForcedSampleCount		= 0;
	psoDesc.RasterizerState.ConservativeRaster		= D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	psoDesc.BlendState.AlphaToCoverageEnable		= FALSE;
	psoDesc.BlendState.IndependentBlendEnable		= FALSE;

	psoDesc.BlendState.RenderTarget[0].BlendEnable		= FALSE;
	psoDesc.BlendState.RenderTarget[0].LogicOpEnable	= FALSE;
	psoDesc.BlendState.RenderTarget[0].SrcBlend			= D3D12_BLEND_ONE;
	psoDesc.BlendState.RenderTarget[0].DestBlend		= D3D12_BLEND_ZERO;
	psoDesc.BlendState.RenderTarget[0].BlendOp			= D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha	= D3D12_BLEND_ONE;
	psoDesc.BlendState.RenderTarget[0].DestBlendAlpha	= D3D12_BLEND_ZERO;
	psoDesc.BlendState.RenderTarget[0].BlendOpAlpha		= D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[0].LogicOp			= D3D12_LOGIC_OP_NOOP;
	psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	psoDesc.DepthStencilState.DepthEnable		= TRUE;
	psoDesc.DepthStencilState.DepthWriteMask	= D3D12_DEPTH_WRITE_MASK_ALL;
	psoDesc.DepthStencilState.DepthFunc			= D3D12_COMPARISON_FUNC_LESS;
	psoDesc.DepthStencilState.StencilEnable		= FALSE;
	psoDesc.DepthStencilState.StencilReadMask	= 0xff;
	psoDesc.DepthStencilState.StencilWriteMask	= 0xff;
	const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
	{ D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
	psoDesc.DepthStencilState.FrontFace = defaultStencilOp;
	psoDesc.DepthStencilState.BackFace	= defaultStencilOp;

	psoDesc.SampleMask				= 0xffffffff;
	psoDesc.PrimitiveTopologyType	= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets		= 1;
	psoDesc.RTVFormats[0]			= DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count		= 1;
	psoDesc.SampleDesc.Quality		= 0;
	psoDesc.DSVFormat				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	
	sm_DefaultDesc = psoDesc;
}

void roGraphicsPipelineStateObjects::WireframeDesc() {
	auto psoDesc = sm_DefaultDesc;
	psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	sm_WireframeDesc = psoDesc;
}

void roGraphicsPipelineStateObjects::TransparentDesc() {

}

void roGraphicsPipelineStateObjects::Initialize() {
	//position only
	sm_InputElemDescPos.SemanticName				= "POSITION";
	sm_InputElemDescPos.SemanticIndex				= 0;
	sm_InputElemDescPos.AlignedByteOffset			= 0;
	sm_InputElemDescPos.Format						= DXGI_FORMAT_R32G32B32_FLOAT;
	sm_InputElemDescPos.InputSlot					= 0;
	sm_InputElemDescPos.InputSlotClass				= D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	sm_InputElemDescPos.InstanceDataStepRate		= 0;
	//position and color
	sm_InputElemDescPosColor[0].SemanticName		= "POSITION";
	sm_InputElemDescPosColor[0].SemanticIndex		= 0;
	sm_InputElemDescPosColor[0].AlignedByteOffset	= 0;
	sm_InputElemDescPosColor[0].Format				= DXGI_FORMAT_R32G32B32_FLOAT;
	sm_InputElemDescPosColor[0].InputSlot			= 0;
	sm_InputElemDescPosColor[0].InputSlotClass		= D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	sm_InputElemDescPosColor[0].InstanceDataStepRate= 0;

	sm_InputElemDescPosColor[1].SemanticName		= "COLOR";
	sm_InputElemDescPosColor[1].SemanticIndex		= 0;
	sm_InputElemDescPosColor[1].AlignedByteOffset	= D3D12_APPEND_ALIGNED_ELEMENT;
	sm_InputElemDescPosColor[1].Format				= DXGI_FORMAT_R32G32B32A32_FLOAT;
	sm_InputElemDescPosColor[1].InputSlot			= 0;
	sm_InputElemDescPosColor[1].InputSlotClass		= D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	sm_InputElemDescPosColor[1].InstanceDataStepRate= 0;

	//position and normal
	sm_InputElemDescPosNormal[0].SemanticName = "POSITION";
	sm_InputElemDescPosNormal[0].SemanticIndex = 0;
	sm_InputElemDescPosNormal[0].AlignedByteOffset = 0;
	sm_InputElemDescPosNormal[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	sm_InputElemDescPosNormal[0].InputSlot = 0;
	sm_InputElemDescPosNormal[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	sm_InputElemDescPosNormal[0].InstanceDataStepRate = 0;

	sm_InputElemDescPosNormal[1].SemanticName = "NORMAL";
	sm_InputElemDescPosNormal[1].SemanticIndex = 0;
	sm_InputElemDescPosNormal[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	sm_InputElemDescPosNormal[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	sm_InputElemDescPosNormal[1].InputSlot = 0;
	sm_InputElemDescPosNormal[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	sm_InputElemDescPosNormal[1].InstanceDataStepRate = 0;

	//position normal and text0
	sm_InputElemDescPosNormalTex[0].SemanticName = "POSITION";
	sm_InputElemDescPosNormalTex[0].SemanticIndex = 0;
	sm_InputElemDescPosNormalTex[0].AlignedByteOffset = 0;
	sm_InputElemDescPosNormalTex[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	sm_InputElemDescPosNormalTex[0].InputSlot = 0;
	sm_InputElemDescPosNormalTex[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	sm_InputElemDescPosNormalTex[0].InstanceDataStepRate = 0;

	sm_InputElemDescPosNormalTex[1].SemanticName = "NORMAL";
	sm_InputElemDescPosNormalTex[1].SemanticIndex = 0;
	sm_InputElemDescPosNormalTex[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	sm_InputElemDescPosNormalTex[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	sm_InputElemDescPosNormalTex[1].InputSlot = 0;
	sm_InputElemDescPosNormalTex[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	sm_InputElemDescPosNormalTex[1].InstanceDataStepRate = 0;

	sm_InputElemDescPosNormalTex[2].SemanticName = "TEXTCOORD";
	sm_InputElemDescPosNormalTex[2].SemanticIndex = 0;
	sm_InputElemDescPosNormalTex[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	sm_InputElemDescPosNormalTex[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	sm_InputElemDescPosNormalTex[2].InputSlot = 0;
	sm_InputElemDescPosNormalTex[2].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	sm_InputElemDescPosNormalTex[2].InstanceDataStepRate = 0;

	//Load starter pack defaults
	DefaultDesc();
	WireframeDesc();
	TransparentDesc();
}

void roGraphicsPipelineStateObjects::CreatePSOs(roPSOType psotype) {

	switch (psotype)
	{
	case enumPsoWireless:
		CreateWireframePSO();
		break;
	case enumPsoOpaqueVertexColors:
		CreateOpaqueVertexColorsPSO();
		break;
	case enumPsoOpaqueLightMat:
		CreateOpaqueLightMatPSO();
		break;
	case enumPsoOpaqueLines:
		CreateOpaqueLinesPSO();
		break;
	default:
		THROW(L"No pso created");
		break;
	}
}

void roGraphicsPipelineStateObjects::CreatePSOFromDesc(std::string name, D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc) {

	auto device = roRootGraphics::sm_Device;
	ID3D12PipelineState* pso;

	HR(device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pso)));
	roGraphicsResourceManager::StoreResource(GR_TYPE_PSO, pso, name);
}

void roGraphicsPipelineStateObjects::CreateWireframePSO() {
	
	auto psoDesc	= sm_WireframeDesc;

	auto rootSig	= roGraphicsResourceGet::RootSignature("RootSig0");
	auto vertexshader	= roGraphicsResourceGet::ShaderByteCode("roWireframeVS");
	auto pixelshader	= roGraphicsResourceGet::ShaderByteCode("roWireframePS");

	psoDesc.InputLayout.pInputElementDescs	= &sm_InputElemDescPos;
	psoDesc.InputLayout.NumElements			= 1;
	psoDesc.pRootSignature = rootSig;
	psoDesc.VS = { reinterpret_cast<BYTE*>(vertexshader->GetBufferPointer()), vertexshader->GetBufferSize() };
	psoDesc.PS = { reinterpret_cast<BYTE*>(pixelshader->GetBufferPointer()), pixelshader->GetBufferSize() };

	CreatePSOFromDesc("WireframePSO", psoDesc);
}

void roGraphicsPipelineStateObjects::CreateOpaqueVertexColorsPSO() {
 
	auto psoDesc = sm_DefaultDesc;

	auto rootSig = roGraphicsResourceGet::RootSignature("RootSig0");
	auto vertexshader = roGraphicsResourceGet::ShaderByteCode("roVertexColorVS");
	auto pixelshader = roGraphicsResourceGet::ShaderByteCode("roVertexColorPS");

	psoDesc.InputLayout.pInputElementDescs = sm_InputElemDescPosColor;
	psoDesc.InputLayout.NumElements = 2;
	psoDesc.pRootSignature = rootSig;
	psoDesc.VS = { reinterpret_cast<BYTE*>(vertexshader->GetBufferPointer()), vertexshader->GetBufferSize() };
	psoDesc.PS = { reinterpret_cast<BYTE*>(pixelshader->GetBufferPointer()), pixelshader->GetBufferSize() };

	CreatePSOFromDesc("VertexColorPSO", psoDesc);
}

void roGraphicsPipelineStateObjects::CreateOpaqueLightMatPSO() {

	auto psoDesc = sm_DefaultDesc;

	auto rootSig = roGraphicsResourceGet::RootSignature("RootSig0");
	auto vertexshader = roGraphicsResourceGet::ShaderByteCode("roLightingMatVS");
	auto pixelshader = roGraphicsResourceGet::ShaderByteCode("roLightingMatPS");

	psoDesc.InputLayout.pInputElementDescs = sm_InputElemDescPosNormal;
	psoDesc.InputLayout.NumElements = 2;
	psoDesc.pRootSignature = rootSig;
	psoDesc.VS = { reinterpret_cast<BYTE*>(vertexshader->GetBufferPointer()), vertexshader->GetBufferSize() };
	psoDesc.PS = { reinterpret_cast<BYTE*>(pixelshader->GetBufferPointer()), pixelshader->GetBufferSize() };

	CreatePSOFromDesc("OpaqueLightingMatPSO", psoDesc);
}

void roGraphicsPipelineStateObjects::CreateOpaqueLinesPSO() {

	auto psoDesc = sm_DefaultDesc;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

	auto rootSig = roGraphicsResourceGet::RootSignature("RootSig0");
	auto vertexshader = roGraphicsResourceGet::ShaderByteCode("roWireframeVS");
	auto pixelshader = roGraphicsResourceGet::ShaderByteCode("roWireframePS");

	psoDesc.InputLayout.pInputElementDescs = &sm_InputElemDescPos;
	psoDesc.InputLayout.NumElements = 1;
	psoDesc.pRootSignature = rootSig;
	psoDesc.VS = { reinterpret_cast<BYTE*>(vertexshader->GetBufferPointer()), vertexshader->GetBufferSize() };
	psoDesc.PS = { reinterpret_cast<BYTE*>(pixelshader->GetBufferPointer()), pixelshader->GetBufferSize() };

	CreatePSOFromDesc("LinesPSO", psoDesc);
}

D3D12_GRAPHICS_PIPELINE_STATE_DESC roGraphicsPipelineStateObjects::sm_DefaultDesc{};
D3D12_GRAPHICS_PIPELINE_STATE_DESC roGraphicsPipelineStateObjects::sm_WireframeDesc{};
D3D12_GRAPHICS_PIPELINE_STATE_DESC roGraphicsPipelineStateObjects::sm_TransparentDesc{};

D3D12_INPUT_ELEMENT_DESC roGraphicsPipelineStateObjects::sm_InputElemDescPos;
D3D12_INPUT_ELEMENT_DESC roGraphicsPipelineStateObjects::sm_InputElemDescPosColor[2];
D3D12_INPUT_ELEMENT_DESC roGraphicsPipelineStateObjects::sm_InputElemDescPosNormal[2];
D3D12_INPUT_ELEMENT_DESC roGraphicsPipelineStateObjects::sm_InputElemDescPosNormalTex[3];