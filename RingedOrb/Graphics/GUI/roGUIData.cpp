#include "roGUIData.h"

void roGUIData::AddGUIMeshName(std::string modeldata) {
	std::pair<std::string, bool> dat;
	dat.first = modeldata;
	dat.second = false;
	sm_MeshData.push_back(dat);
}

std::vector<std::pair<std::string, bool>> roGUIData::sm_MeshData;

bool roGUIData::GetOpenModelDialogState() {
	bool currState = sm_bIsOpenModelDialog;
	if (sm_bIsOpenModelDialog)
		SetOpenModelDialogState(false);
	return currState;
}

void roGUIData::SetOpenModelDialogState(bool state) {
	sm_bIsOpenModelDialog = state;
 }

bool roGUIData::GetAddEnityState() {
	bool currState = sm_bIsAddEntity;
	if (sm_bIsAddEntity)
		SetAddEnityState(false);
	return currState;
}

void roGUIData::SetAddEnityState(bool state) {
	sm_bIsAddEntity = state;
}

bool roGUIData::sm_bIsOpenModelDialog = false;
bool roGUIData::sm_bIsAddEntity = false;