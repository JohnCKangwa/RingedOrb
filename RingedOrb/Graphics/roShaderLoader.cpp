#include "roShaderLoader.h"
#include "roGraphicsResources.h"
#include "..\Core\roFileSystem.h"

std::wstring const ShaderImmediateDirectory[5]{ L"\\VS", L"\\HS", L"\\DS", L"\\GS", L"\\PS" };
std::string const ShaderEntryPoints[5]{ "VS", "HS", "DS", "GS", "PS" };
std::string const ShaderTarget[5]{ "vs_5_1", "hs_5_1", "ds_5_1", "gs_5_1","ps_5_1" };

void LoadShader::Initialize() {
	
	std::wstring path_HLSL = RingedOrbFileSystem::g_ShadersRootDir + L"\\HLSL";
	std::wstring tempPath;

	for (int i = 0; i < (int)ShaderType::COUNT; ++i) {
		tempPath = path_HLSL + ShaderImmediateDirectory[i];
		std::vector<std::wstring> s_names = RingedOrbFileSystem::GetFileFileNamesFromPath(tempPath);

		for (auto iter : s_names) {
			roFileNameCache fnc;
			fnc.name = iter;
			fnc.type = (ShaderType(i));
			sm_ShaderFileNames.push_back(fnc);
		}
	}

	//load shaders in shader directories
	for (auto iter : sm_ShaderFileNames) {
		std::wstring pathName	= path_HLSL + ShaderImmediateDirectory[(int)iter.type] + L"\\" + iter.name;
		std::string tempName	= ConvertToByteString(iter.name);
		//remove .hlsl and append shader Suffix
		TrimStringFromRight(tempName, 5);
		tempName += ShaderEntryPoints[(int)iter.type];

		std::string entryPoint	= ShaderEntryPoints[iter.type];
		std::string target		= ShaderTarget[(int)iter.type];

		FromFile(pathName, tempName, iter.type, 0, entryPoint, target);
	}
}

void LoadShader::FromFile(std::wstring fileDir, std::string shaderName, ShaderType shaderType, const D3D_SHADER_MACRO* defines,
	const std::string entrypoint, const std::string target) {

	UINT shaderflags = 0;
	ID3DBlob* shader = nullptr;

#if defined(DEBUG) || defined(_DEBUG)
	shaderflags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif		
	ID3DBlob* error = nullptr;

	D3DCompileFromFile(fileDir.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entrypoint.c_str(), target.c_str(), shaderflags,
		0, &shader, &error);

	if (error) {
		OutputDebugStringA((char*)error->GetBufferPointer());
		SAFE_RELEASE(error)
	}

	if (shader == nullptr)
		THROW(L"failed to load shader");

	roGraphicsResourceManager::StoreResource(GR_TYPE_SHADER, shader, shaderName);
}

void LoadShader::FromByteCode(std::wstring fileDir, std::string shaderName, ShaderType shaderType) {
	//unused at the moment
	ID3DBlob* shader;

	HR(D3DReadFileToBlob(fileDir.c_str(), &shader));
	roGraphicsResourceManager::StoreResource(GR_TYPE_SHADER, shader, shaderName);
}

std::vector<roFileNameCache> LoadShader::sm_ShaderFileNames;