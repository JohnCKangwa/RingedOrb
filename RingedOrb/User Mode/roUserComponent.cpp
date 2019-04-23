#include "roUserComponent.h"
#include "..\Graphics\GUI\roGUIData.h"
#include "..\Core\roWindow.h"
#include "..\Core\roFileSystem.h"

roUserComponent::roUserComponent(roEventQueue* eventQueue) : roEventNode(eventQueue) {
	roEventNode::SubscribeForEvent(roEVENT_ID::E_ID_MODEL);
}

void roUserComponent::OnEvent(roEvent* evt) {
	if (evt->EventID == E_ID_MODEL) {
		if(((roModelEvent*)evt)->Type == roModelEvent::roEVENT_MODEL_TYPE::STORED)
			roGUIData::AddGUIMeshName(((roModelEvent*)evt)->Name);
	}
}

void roUserComponent::Update() {

	if (roGUIData::GetOpenModelDialogState()) {
		static std::wstring path;
		path = RingedOrbFileSystem::OpenFileDialog(RingedOrbFileSystem::g_ModelsRootDir, roWindow::GetWindowHandle(),
			RingedOrbFileSystem::roFileExtension::F_EXT_OBJ);
		//send message
		roModelEvent modelEvent;
		modelEvent.ModelPath = path;
		modelEvent.Type = roModelEvent::CREATE;

		PushEvent(modelEvent);
	}

	if (roGUIData::GetAddEnityState()) {
		std::string modelName;
		for (auto &iter : roGUIData::sm_MeshData) {
			if (iter.second) {
				modelName = iter.first;
				break;
			}
		}

		roREntityEvent REEvent;
		REEvent.MeshRefName = modelName;
		REEvent.Type = roREntityEvent::CREATE;

		PushEvent(REEvent);
	}
}
