#include "roID_Generator.h"


roID_Generator::roID_Generator() {
	m_CurrentID = 0;
}

UINT64 roID_Generator::GetID() {
	std::lock_guard<std::mutex> lg(m_IDMutex);

	UINT64 ID___ = -1;

	if (!m_UnusedIDs.empty()) {
		ID___ = m_UnusedIDs.back();
		m_UnusedIDs.pop_back();
		return ID___;
	}

	return m_CurrentID++;
}

void roID_Generator::DiscardID(UINT64 id) {
	std::lock_guard<std::mutex> lg(m_IDMutex);

	m_UnusedIDs.push_back(id);
}
