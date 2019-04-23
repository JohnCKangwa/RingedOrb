#pragma once
#include "pch.h"
#include "roEvents.h"

class roEventQueue;

class roEventNode {
public:
	roEventNode(roEventQueue*);
	void SubscribeForEvent(roEVENT_ID);
	void PushEvent(roEvent&);
	virtual void OnEvent(roEvent*) = 0;
public:
	roEventQueue* m_EventQueue;
};

class roEventQueue {
	enum CCDEventMode {CCD_CREATE, CCD_COPY, CCD_DELETE};
	typedef void(roEventNode::* OnEventCallbackFunc)(roEvent*);

public:

 	inline void BindOnEvent(roEVENT_ID eventID, void(roEventNode::* func)(roEvent*), roEventNode* instance) {

		std::pair<OnEventCallbackFunc, void*> listener((OnEventCallbackFunc)func, instance);
		m_EventListeners[eventID].push_back(listener);
	}

	void PushEvent(roEvent& event);

	void DispatchEvents();
	void FreeEvent(roEvent* event);

private:
	void CCD_DispatchableEvent(roEVENT_ID eventID, roEvent*, roEvent**, CCDEventMode);
private:
	
	std::vector<std::pair<OnEventCallbackFunc, void*>> m_EventListeners[E_ID_COUNT];
	std::queue<roEvent*> m_Events[E_ID_COUNT];
	std::queue<roEvent*> m_CachedEvents[E_ID_COUNT];
	std::vector<roEvent*> m_EventsFreePool[E_ID_COUNT];
	std::vector<roEvent*> m_EventsAllocator[E_ID_COUNT];

	bool m_bDispatching = false;
	std::mutex m_EventMutex;
};

