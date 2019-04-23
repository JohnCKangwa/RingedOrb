#include "roGraphicsResources.h"
#include "..\Core\roWindow.h"

bool roGraphicsResourceManager::FindGraphicsResource(void** resourceGet, RO_GRAPHICS_RESOURCE_TYPE type, std::string name, bool bGetrsc) {

	std::lock_guard<std::mutex> lg(sm_ResourceMutex[type]);

	auto resource = sm_Resources[type].find(name);

 	if (resource == sm_Resources[type].end()) {
		if (bGetrsc) {
			std::wstring errMrssage = L"Couldnt find " + g_RscNames[type];
			MessageBox(roWindow::GetWindowHandle(), errMrssage.c_str(), L"Resource not found", 0);
			*resourceGet = nullptr;
		}
		return false;
	}
	else {
		if(bGetrsc)
			*resourceGet = resource->second;
		return true;
	}
}

void roGraphicsResourceManager::StoreResource(RO_GRAPHICS_RESOURCE_TYPE type, void* resource, std::string name) {

	std::lock_guard<std::mutex> lg(sm_ResourceMutex[type]);
	sm_Resources[type][name] = resource;
}

void roGraphicsResourceManager::Shutdown() {

	for (int i = 0; i < 3; i++) {
		for (auto iter : sm_Resources[(RO_GRAPHICS_RESOURCE_TYPE)i]) {
			((IUnknown*)iter.second)->Release();
			iter.second = nullptr;
		}
		sm_Resources[(RO_GRAPHICS_RESOURCE_TYPE)i].clear();
	}
	   
	roMesh* meshHandle = nullptr;
	for (auto iter : sm_Resources[GR_TYPE_MESH]) {
		meshHandle = (roMesh*)iter.second;
		meshHandle->Destroy();
		SAFE_DELETE(meshHandle);
	}
	sm_Resources[GR_TYPE_MESH].clear();
}

std::unordered_map<std::string, void*> roGraphicsResourceManager::sm_Resources[GR_TYPE_COUNT];
std::mutex roGraphicsResourceManager::sm_ResourceMutex[GR_TYPE_COUNT];

//resource loader
bool roGraphicsResourceLoader::IsCreated = false;

void roGraphicsResourceLoader::LoadDefaults() {
	if (IsCreated)
		return;
	IsCreated = true;

	LoadRootSignatures();
	LoadShaders();

	//Load PSOs on multiple threads
	roGraphicsPipelineStateObjects::Initialize();

	auto psoLoader = [&](int index)->void { LoadPSO(((roPSOType)index)); };
	std::vector<std::thread> psoCreateWorker;
	for (int i = 0; i < roPSOType::enumPsoCount; i++) {
		psoCreateWorker.push_back(std::thread(psoLoader, i));
	}
	//
	for (int i = 0; i < psoCreateWorker.size(); i++)
		psoCreateWorker[i].join();

	IsCreated = true;
}

void roGraphicsResourceLoader::LoadMesh(roMesh& mesh, std::string name) {
	
	if (roGraphicsResourceManager::FindGraphicsResource(nullptr, GR_TYPE_MESH, name, false)) {
		mesh.Destroy();
		MessageBox(roWindow::GetWindowHandle(), L"mesh resouce already exists", L"mesh Exists", 0);
		return;
	}
	roMesh* pMesh = new roMesh();
	*pMesh = mesh;
	roGraphicsResourceManager::StoreResource(GR_TYPE_MESH, pMesh, name);
}

void roGraphicsResourceLoader::LoadMaterial(roMaterial& material, std::string name) {

	if (roGraphicsResourceManager::FindGraphicsResource(nullptr, GR_TYPE_MATERIAL, name, false)) {
		MessageBox(roWindow::GetWindowHandle(), L"matrial resouce already exists", L"Material Exists", 0);
		return;
	}
	roMaterial* pMaterial = new roMaterial();
	*pMaterial = material;
	roGraphicsResourceManager::StoreResource(GR_TYPE_MATERIAL, pMaterial, name);
}

void roGraphicsResourceLoader::LoadPSO(roPSOType type) {
	if (!IsCreated)
		THROW(L"Did not call in Initialize");

	roGraphicsPipelineStateObjects::CreatePSOs(type);
	//add pso load called
}

void roGraphicsResourceLoader::LoadRootSignatures() {
	roGraphicsRootSignature::Initialize();
}

void roGraphicsResourceLoader::LoadShaders() {
	LoadShader::Initialize();
}

void roGraphicsResourceLoader::LoadMeshes() {

}

ID3DBlob* roGraphicsResourceGet::ShaderByteCode(std::string name) {
	ID3DBlob* shaderbyteCode = nullptr;
	roGraphicsResourceManager::FindGraphicsResource((void**)&shaderbyteCode, GR_TYPE_SHADER, name);
	return shaderbyteCode;
}

ID3D12RootSignature* roGraphicsResourceGet::RootSignature(std::string name) {
	ID3D12RootSignature* rootSig = nullptr;
	roGraphicsResourceManager::FindGraphicsResource((void**)&rootSig, GR_TYPE_ROOT_SIG, name);
	return rootSig;
}

ID3D12PipelineState* roGraphicsResourceGet::PSO(std::string name) {
	ID3D12PipelineState* pso = nullptr;
	roGraphicsResourceManager::FindGraphicsResource((void**)&pso, GR_TYPE_PSO, name);
	return pso;
}

roMesh* roGraphicsResourceGet::Mesh(std::string name) {
	roMesh* mesh = nullptr;
	roGraphicsResourceManager::FindGraphicsResource((void**)&mesh, GR_TYPE_MESH, name);
	return mesh;
}

roMaterial* roGraphicsResourceGet::Material(std::string name) {
	roMaterial* material = nullptr;
	roGraphicsResourceManager::FindGraphicsResource((void**)&material, GR_TYPE_MATERIAL, name);
	return material;
}

void roGraphicsResourceCleanUp::Shutdown() {
	roGraphicsResourceManager::Shutdown();
}
