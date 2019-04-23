#pragma once
#include "..\Core\pch.h"
#include "DX12IncludeFile.h"
#include "roShaderLoader.h"
#include "roGraphicsPipelineStateObject.h"
#include "roGraphicsRootSignature.h"
#include "roRenderableEntity.h"
#include "roMaterial.h"

enum RO_GRAPHICS_RESOURCE_TYPE {
	GR_TYPE_SHADER = 0,
	GR_TYPE_PSO = 1,
	GR_TYPE_ROOT_SIG = 2,
	GR_TYPE_MESH = 3,
	GR_TYPE_MATERIAL = 4,
	GR_TYPE_COUNT = 5
};

static std::wstring g_RscNames[GR_TYPE_COUNT] = { L"shader", L"pso", L"root signature", L"mesh", L"material" };

class roGraphicsResourceManager {

	friend struct LoadShader;
	friend class roGraphicsPipelineStateObjects;
	friend class roGraphicsRootSignature;
	friend struct roGraphicsResourceLoader;
	friend struct roGraphicsResourceGet;
	friend struct roGraphicsResourceCleanUp;

private:
	static bool FindGraphicsResource(void**, RO_GRAPHICS_RESOURCE_TYPE type, std::string name, bool getRsc = true);
private:
	static void StoreResource(RO_GRAPHICS_RESOURCE_TYPE type, void* resource, std::string name);
	static void Shutdown();

private:
	static std::unordered_map<std::string, void*> sm_Resources[GR_TYPE_COUNT];
	static std::mutex sm_ResourceMutex[GR_TYPE_COUNT];
};
//resource management objects
struct roGraphicsResourceLoader {

	static void LoadDefaults(); 
	static void LoadMesh(roMesh& mesh, std::string name);
	static void LoadMaterial(roMaterial& material, std::string name);

private:
	//only load single PSO per thread to support multiple loads
	static void LoadPSO(roPSOType type); 

private:
	static void LoadRootSignatures(); 
	static void LoadShaders(); 
	static void LoadMeshes();
private:
	static bool IsCreated;
};

struct roGraphicsResourceGet {
	static ID3DBlob* ShaderByteCode(std::string name);
	static ID3D12RootSignature* RootSignature(std::string name);
	static ID3D12PipelineState* PSO(std::string name);
	static roMesh* Mesh(std::string name);
	static roMaterial* Material(std::string name);
	
};

struct roGraphicsResourceCleanUp {
	static void Shutdown();
};
