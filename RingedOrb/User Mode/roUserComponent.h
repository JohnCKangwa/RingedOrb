#pragma once
#include "..\Core\roEventSystem.h"


class roUserComponent : public roEventNode {
public:
	roUserComponent(roEventQueue* eventQueue);
	void OnEvent(roEvent*) override;
	void Update();

};
