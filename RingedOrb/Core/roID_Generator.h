#pragma once
#include "pch.h"

struct roID_Generator {
	roID_Generator();

	UINT64 GetID();
	void DiscardID(UINT64 id);
private:
	std::vector<UINT64> m_UnusedIDs;
	UINT64 m_CurrentID = 0;
	std::mutex m_IDMutex;
};
