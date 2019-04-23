#include "roEvents.h"

roEvent::roEvent(roEVENT_ID eventid) : EventID(eventid){}

roMoueseEvent::roMoueseEvent() : roEvent(E_ID_MOUSE) {}

roKeyboardEvent::roKeyboardEvent() : roEvent(E_ID_KEYBOARD) {}

roModelEvent::roModelEvent() : roEvent(E_ID_MODEL) {}

roREntityEvent::roREntityEvent() : roEvent(E_ID_RENTITY) {}
