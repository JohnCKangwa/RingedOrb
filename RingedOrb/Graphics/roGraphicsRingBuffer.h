#pragma once
#include "..\Core\pch.h"


class roGraphicsRingBuffer {

	struct TailFrameInfo {
		UINT64 BufferOffset = 0;
		uint64_t FenceValue = 0;
	};

protected:
	roGraphicsRingBuffer(UINT64 fixedBlockSize, UINT64 bufferSize);
	UINT64 Allocate(UINT64 memsize);
	void MarkTail(int frameIdx);

private:
	bool MemAvailable(UINT64 elementSize);
	UINT64 FreeUpMem(UINT64 memsize);
	UINT64 GetOldestTailMemOffset();

	std::deque<TailFrameInfo> m_TailFrames;

	UINT64 m_BufferSize = 0;
	UINT64 m_HeadOffset = 0;
	UINT64 m_TailOffset = 0;
	UINT64 m_FixedBlockEndFromStart = 0;
	UINT64 m_FreeMemory;

	std::mutex m_RingBufferMutex;
	bool m_TailMoved = false;
};