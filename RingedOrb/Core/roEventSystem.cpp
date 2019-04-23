#include "roEventSystem.h"

roEventNode::roEventNode(roEventQueue* eventqueue) : m_EventQueue(eventqueue) {}

void roEventNode::SubscribeForEvent(roEVENT_ID eventid) {
	m_EventQueue->BindOnEvent(eventid, &roEventNode::OnEvent, this);
}

void roEventNode::PushEvent(roEvent& dispatchableEvent) {
	m_EventQueue->PushEvent(dispatchableEvent);
}

void roEventQueue::PushEvent(roEvent& dispatchableEvent) {

	roEvent* evt = nullptr;

	if (m_EventsFreePool[dispatchableEvent.EventID].empty()) {
		CCD_DispatchableEvent(dispatchableEvent.EventID, &dispatchableEvent, &evt, CCDEventMode::CCD_CREATE);
		m_EventsAllocator[dispatchableEvent.EventID].push_back(evt);
	}
	else {
		evt = m_EventsFreePool->back();
		m_EventsFreePool[dispatchableEvent.EventID].pop_back();
		CCD_DispatchableEvent(dispatchableEvent.EventID, &dispatchableEvent, &evt, CCDEventMode::CCD_COPY);
	}

	if (m_bDispatching) {
		m_CachedEvents[evt->EventID].push(evt);
	}
	else {
		std::lock_guard<std::mutex> lg(m_EventMutex);
		m_Events[evt->EventID].push(evt);
	}
}

void roEventQueue::DispatchEvents() {
	m_bDispatching = true;
	std::lock_guard<std::mutex> lg(m_EventMutex);

	for (UINT eventQueueIdx = 0; eventQueueIdx < E_ID_COUNT; eventQueueIdx++) {
		auto &eventQueue = m_Events[eventQueueIdx];

		if (eventQueue.empty())
			continue;

		UINT eventQueueSize = (UINT)eventQueue.size();
		auto &eventListeners = m_EventListeners[eventQueueIdx];

		if (eventListeners.empty()) {
			
			while (!eventQueue.empty()) {
				eventQueue.pop();
			}
			continue;
		}

		while (!eventQueue.empty()) {
			auto &currEvent = eventQueue.front();

			for (auto &listener : eventListeners) {
				auto instance = listener.second;
				auto instanceFunc = listener.first;

				(((roEventNode*)instance)->*instanceFunc)(currEvent);
			}
			FreeEvent(currEvent);
			eventQueue.pop();
		}
	}
	m_bDispatching = false;
	//resolve cached events
	for (int i = 0; i < E_ID_COUNT; i++) {
		m_Events[i] = m_CachedEvents[i];
		while (!m_CachedEvents[i].empty()) {
			m_CachedEvents[i].pop();
		}
	}
}

void roEventQueue::FreeEvent(roEvent* dispatchableEvent) {
	m_EventsFreePool->push_back(dispatchableEvent);
}

void roEventQueue::CCD_DispatchableEvent(roEVENT_ID eventID, roEvent* sourcedispatchableEvent, roEvent** destdispatchableEvent,
	CCDEventMode ccdevtmode) {

	if (ccdevtmode == CCDEventMode::CCD_CREATE) {
		switch (eventID) {
		case E_ID_MOUSE:
			break;
		case E_ID_KEYBOARD:
			break;
		case E_ID_MODEL:
			*destdispatchableEvent = new roModelEvent();
			*((roModelEvent*)*destdispatchableEvent) = *((roModelEvent*)sourcedispatchableEvent);
			break;
		case E_ID_RENTITY:
			*destdispatchableEvent = new roREntityEvent();
			*((roREntityEvent*)*destdispatchableEvent) = *((roREntityEvent*)sourcedispatchableEvent);
			break;
		case E_ID_COUNT:
			break;
		default:
			break;
		}
	}
	else if (ccdevtmode == CCDEventMode::CCD_COPY) {
		switch (eventID) {
		case E_ID_MOUSE:
			break;
		case E_ID_KEYBOARD:
			break;
		case E_ID_MODEL:
			*((roModelEvent*)*destdispatchableEvent) = *((roModelEvent*)sourcedispatchableEvent);
			break;
		case E_ID_RENTITY:
			*((roREntityEvent*)*destdispatchableEvent) = *((roREntityEvent*)sourcedispatchableEvent);
			break;
		case E_ID_COUNT:
			break;
		default:
			break;
		}
	}
}

