#pragma once
#include <strsafe.h>
#include <string.h>
#include <vector>

namespace RingedOrbFileSystem {
	
	extern std::wstring g_ShadersRootDir;
	extern std::wstring g_TexturesRootDir;
	extern std::wstring g_ModelsRootDir;

	enum roFileExtension{F_EXT_OBJ, F_EXT_HLSL, F_EXT_ALL};

	std::wstring OpenFileDialog(std::wstring initialPath, void* pHandleOwner, roFileExtension fileExt = F_EXT_ALL);

	std::vector<std::wstring> GetFileFileNamesFromPath(std::wstring path);
}