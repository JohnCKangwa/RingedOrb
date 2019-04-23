#pragma once
#include "DX12IncludeFile.h"
#include "roGraphicsResources.h"




class roGraphicsRootSignature {
public:
	static void Initialize();

private:
	static void CreateRootSignature(std::string name, D3D12_ROOT_SIGNATURE_DESC* rootDesc);
	static void InitEmptyRootSig();
	static void InitRootSig0();
	static void InitRootSig1();


	static void LogErrors(ID3DBlob* errors);
private:
	static bool sm_IsCreated;
};
