#pragma once
#include "..\Core\pch.h"
#include "DX12IncludeFile.h"
#include <d3dcompiler.h>
#include "..\Core\roErrorHandling.h"	
#include "roGraphicsResources.h"


enum ShaderType { VS = 0, HS = 1, DS = 2, GS = 3, PS = 4, COUNT = 5 };

struct roFileNameCache {
	std::wstring	name;
	ShaderType		type;
};


struct LoadShader {
	friend struct roGraphicsResourceLoader;
private:
	static void Initialize();
	static void FromFile(std::wstring fileDir, std::string shaderName, ShaderType shaderType, const D3D_SHADER_MACRO* defines, 
		const std::string entrypoint, const std::string target); 	

	static void FromByteCode(std::wstring fileDir, std::string shaderName, ShaderType shaderType);

private:
	static std::vector<roFileNameCache>  sm_ShaderFileNames;
};