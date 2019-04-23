#include "roGraphicsRootSignature.h"
#include "DX12IncludeFile.h"
#include "roRootGraphics.h"
#include "..\Core\roErrorHandling.h"


void roGraphicsRootSignature::Initialize() {
	if (sm_IsCreated) {
		MessageBox(0, L"RootSig already Initialized", L"Well defined", MB_OK);
		return;
	}
	InitEmptyRootSig();
	InitRootSig0();
	InitRootSig1();

	sm_IsCreated = true;
}

void roGraphicsRootSignature::CreateRootSignature(std::string name, D3D12_ROOT_SIGNATURE_DESC *rootDesc) {

	ID3DBlob* blob = nullptr;
	ID3DBlob* errors = nullptr;
	ID3D12RootSignature* rootSignature;

	D3D12SerializeRootSignature(rootDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &blob, &errors);
	LogErrors(errors);
	HR(roRootGraphics::sm_Device->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));
	roGraphicsResourceManager::StoreResource(GR_TYPE_ROOT_SIG, rootSignature, name);

	SAFE_RELEASE(blob)
}

void roGraphicsRootSignature::InitEmptyRootSig() {

	D3D12_ROOT_SIGNATURE_DESC rootDesc;
	rootDesc.pParameters		= 0;
	rootDesc.NumParameters		= 0;
	rootDesc.NumStaticSamplers	= 0;
	rootDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	
	CreateRootSignature("RootSigEmpty", &rootDesc);
}

void roGraphicsRootSignature::InitRootSig0() {

 	//object cb
	D3D12_ROOT_PARAMETER rootParams[3];
	rootParams[0].ParameterType				= D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParams[0].Descriptor.ShaderRegister = 0;
	rootParams[0].Descriptor.RegisterSpace	= 0;
	rootParams[0].ShaderVisibility			= D3D12_SHADER_VISIBILITY_VERTEX;
	//material cb
	rootParams[1].ParameterType				= D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParams[1].Descriptor.ShaderRegister = 1;
	rootParams[1].Descriptor.RegisterSpace	= 0;
	rootParams[1].ShaderVisibility			= D3D12_SHADER_VISIBILITY_ALL;
	//per frame cb
	rootParams[2].ParameterType				= D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParams[2].Descriptor.ShaderRegister = 2;
	rootParams[2].Descriptor.RegisterSpace	= 0;
	rootParams[2].ShaderVisibility			= D3D12_SHADER_VISIBILITY_ALL;
	
	D3D12_ROOT_SIGNATURE_DESC rootDesc;
	rootDesc.pParameters = rootParams;
	rootDesc.NumParameters = 3;
	rootDesc.NumStaticSamplers = 0;
	rootDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	CreateRootSignature("RootSig0", &rootDesc);
 }

void roGraphicsRootSignature::InitRootSig1() {

	D3D12_ROOT_PARAMETER rootParams[4];
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParams[0].Descriptor.ShaderRegister = 0;
	rootParams[0].Descriptor.RegisterSpace = 0;
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	//material cb
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParams[1].Descriptor.ShaderRegister = 1;
	rootParams[1].Descriptor.RegisterSpace = 0;
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	//per frame cb
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParams[2].Descriptor.ShaderRegister = 2;
	rootParams[2].Descriptor.RegisterSpace = 0;
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	//texture descriptor range. only one
	D3D12_DESCRIPTOR_RANGE descriptorRange;
	descriptorRange.BaseShaderRegister = 0;
	descriptorRange.NumDescriptors = 1;
	descriptorRange.OffsetInDescriptorsFromTableStart = 0;
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.RegisterSpace = 0;
	//single range and signle descriptor for texture range
	rootParams[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParams[3].DescriptorTable.NumDescriptorRanges = 1;
	rootParams[3].DescriptorTable.pDescriptorRanges = &descriptorRange;
	rootParams[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_DESC rootDesc;
	rootDesc.pParameters = rootParams;
	rootDesc.NumParameters = 4;
	rootDesc.NumStaticSamplers = 0;
	rootDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	CreateRootSignature("RootSig1", &rootDesc);
}

void roGraphicsRootSignature::LogErrors(ID3DBlob* errors) {
	if (errors) {
		OutputDebugStringA((char*)errors->GetBufferPointer());
		SAFE_RELEASE(errors)
	}
}

bool roGraphicsRootSignature::sm_IsCreated = false;
