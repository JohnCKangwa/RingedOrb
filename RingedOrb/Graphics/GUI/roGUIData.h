#pragma once
#include "..\..\Core\pch.h"

struct roGUIREntity {
	UINT ID;
	float Translation[3];
};

struct roGUIData {

	static bool GetOpenModelDialogState();
	static void SetOpenModelDialogState(bool state);
	static bool GetAddEnityState();
	static void SetAddEnityState(bool state);

	static void AddGUIMeshName(std::string);

public:
	static std::vector<std::pair<std::string, bool>> sm_MeshData;
	static std::vector<roGUIREntity> sm_GUIREntities;

private:
	static bool sm_bIsOpenModelDialog;
	static bool sm_bIsAddEntity;
};

