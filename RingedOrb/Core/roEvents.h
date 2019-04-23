#pragma once
#include "..\Core\pch.h"

enum roEVENT_ID { 
	E_ID_MOUSE = 0,
	E_ID_KEYBOARD = 1,
	E_ID_MODEL = 2,
	E_ID_RENTITY = 3,
	E_ID_COUNT = 4 
};

struct roEvent {
	roEvent(roEVENT_ID);
	roEVENT_ID EventID;
};

struct roMoueseEvent : public roEvent {
	roMoueseEvent();
	unsigned int x = 0;
	unsigned int y = 0;
};

struct roKeyboardEvent : public roEvent {
	roKeyboardEvent();
};

struct roModelEvent : public roEvent {
	enum roEVENT_MODEL_TYPE { CREATE, STORED, NONE };
	roModelEvent();
	roEVENT_MODEL_TYPE Type;
	std::wstring ModelPath;
	std::string Name;
};

struct roREntityEvent : public roEvent {
	enum roEVENT_RENTITY_TYPE {CREATE, STORED };
	roREntityEvent();
	roEVENT_RENTITY_TYPE Type;
	UINT ID;
	std::string MeshRefName;
};

