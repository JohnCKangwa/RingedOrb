#include "roFileSystem.h"
#include <Windows.h>
#include "roErrorHandling.h"

std::wstring RingedOrbFileSystem::g_ShadersRootDir = L"Graphics\\Shaders";
std::wstring RingedOrbFileSystem::g_TexturesRootDir = L"Graphics\\Textures";
std::wstring RingedOrbFileSystem::g_ModelsRootDir = L"Graphics\\Models";

LPCWSTR g_ObjFileExt = L"OBJ File\0*.obj\0";
LPCWSTR g_HlslFileExt = L"HLSL File\0*.hlsl\0";
LPCWSTR g_AllFileExt = L"All\0*.*\0";

std::wstring RingedOrbFileSystem::OpenFileDialog(std::wstring initialPath, void* pHandleOwner, roFileExtension fileExt) {

	OPENFILENAME openFileDialog;      
	TCHAR szFile[_MAX_DIR] = { 0 };
	TCHAR szFileTitle[100] = { 0 };
	
	// Initialize 
	ZeroMemory(&openFileDialog, sizeof(openFileDialog));
	openFileDialog.lStructSize = sizeof(openFileDialog);
	openFileDialog.hwndOwner = *((HWND*)&pHandleOwner);
	openFileDialog.lpstrFile = szFile;
	openFileDialog.nMaxFile = sizeof(szFile);
	openFileDialog.lpfnHook = NULL;
	
	switch (fileExt) {
	case roFileExtension::F_EXT_OBJ:
		openFileDialog.lpstrFilter = g_ObjFileExt;
		break;
	case roFileExtension::F_EXT_ALL:
		openFileDialog.lpstrFilter = g_AllFileExt;
		break;
	default:
		break;
	}	

	openFileDialog.nFilterIndex = 1;
	openFileDialog.lpstrFileTitle = NULL;
	openFileDialog.nMaxFileTitle = 0;
	openFileDialog.lpstrInitialDir = initialPath.c_str();
	openFileDialog.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&openFileDialog) == TRUE) {
		 
	}
	else {
		switch (CommDlgExtendedError()) {
		case CDERR_DIALOGFAILURE:
			THROW(L"CDERR_DIALOGFAILURE");
			break;
		case CDERR_FINDRESFAILURE:
			THROW(L"CDERR_FINDRESFAILURE");
			break;
		case CDERR_INITIALIZATION:
			THROW(L"CDERR_INITIALIZATION");
			break;
		case CDERR_LOADRESFAILURE:
			THROW(L"CDERR_LOADRESFAILURE");
			break;
		case CDERR_LOADSTRFAILURE:
			THROW(L"CDERR_LOADSTRFAILURE");
			break;
		case CDERR_LOCKRESFAILURE:
			THROW(L"CDERR_LOCKRESFAILURE");
			break;
		case CDERR_MEMALLOCFAILURE:
			THROW(L"CDERR_MEMALLOCFAILURE");
			break;
		case CDERR_MEMLOCKFAILURE:
			THROW(L"CDERR_MEMLOCKFAILURE");
			break;
		case CDERR_NOHINSTANCE:
			THROW(L"CDERR_NOHINSTANCE");
			break;
		case CDERR_NOHOOK:
			THROW(L"CDERR_NOHOOK");
			break;
		case CDERR_NOTEMPLATE:
			THROW(L"CDERR_NOTEMPLATE");
			break;
		case CDERR_STRUCTSIZE:
			THROW(L"CDERR_STRUCTSIZE");
			break;
		case FNERR_BUFFERTOOSMALL:
			THROW(L"FNERR_BUFFERTOOSMALL");
			break;
		case FNERR_INVALIDFILENAME:
			THROW(L"FNERR_INVALIDFILENAME");
			break;
		case FNERR_SUBCLASSFAILURE:
			THROW(L"FNERR_SUBCLASSFAILURE");
			break;
		default:
			THROW(L"Cancelled operation");
		}
	}

	return szFile;
}

std::vector<std::wstring> RingedOrbFileSystem::GetFileFileNamesFromPath(std::wstring path) {

	WIN32_FIND_DATA findData;
	TCHAR szDir[MAX_PATH];
	HANDLE hFind = INVALID_HANDLE_VALUE;

	StringCchCopy(szDir, MAX_PATH, path.c_str());
	StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

	hFind = FindFirstFile(szDir, &findData);

	if (INVALID_HANDLE_VALUE == hFind) {
		THROW(L"error in file handle")
	}

	std::vector<std::wstring> getNames;

	do {
		if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			getNames.push_back(findData.cFileName);
		}
	} while (FindNextFile(hFind, &findData) != 0);

	FindClose(hFind);

	return getNames;
}

