
#include "roGraphicsRingBuffer.h"
#include "roRootGraphics.h"
#include "roCommandQueue.h"



roGraphicsRingBuffer::roGraphicsRingBuffer(UINT64 fixedBlockSize, UINT64 bufferSize) {

	m_BufferSize = bufferSize;

	m_FixedBlockEndFromStart = fixedBlockSize;
	m_HeadOffset = m_FixedBlockEndFromStart;
	m_TailOffset = m_FixedBlockEndFromStart;

	m_FreeMemory = m_BufferSize - m_FixedBlockEndFromStart;
}

UINT64 roGraphicsRingBuffer::Allocate(UINT64 elementSize) {
	std::lock_guard<std::mutex> lg(m_RingBufferMutex);

	m_TailMoved = true;

	UINT64 freeBlockStart(-1);

	if (MemAvailable(elementSize)) {
		freeBlockStart = m_TailOffset;
		m_TailOffset += elementSize;
		m_FreeMemory = m_FreeMemory - elementSize;

		return freeBlockStart;
	} 
	else {
		freeBlockStart = FreeUpMem(elementSize);
		return freeBlockStart;
	}			
}

void roGraphicsRingBuffer::MarkTail(int frameIdx) {
	//This should be called after current frame is signaled at the end of recording
	if (m_TailMoved) {

		TailFrameInfo tailMarker;
		tailMarker.BufferOffset = m_TailOffset;
		tailMarker.FenceValue = roRootGraphics::sm_FrameContext[frameIdx].FenceValue;

		m_TailFrames.push_back(tailMarker);

		m_TailMoved = false;
	}
}

bool roGraphicsRingBuffer::MemAvailable(UINT64 memsize) {
		if (memsize <= m_FreeMemory)
			return true;
		else
			return false;
}

UINT64 roGraphicsRingBuffer::FreeUpMem(UINT64 memsize) {

	UINT64 freeBlockStart(-1);
	// reached end of memory buffer
	if (m_TailOffset > m_HeadOffset) {

		UINT64 remainingSpaceAtEnd = m_BufferSize - m_TailOffset;
		m_FreeMemory = remainingSpaceAtEnd;

		if (MemAvailable(memsize)) {
			freeBlockStart = m_TailOffset;
			m_TailOffset += memsize;
			m_FreeMemory -= memsize;
			return freeBlockStart;
		}
		else {
			m_TailOffset = m_FixedBlockEndFromStart;
			m_FreeMemory = m_HeadOffset - m_TailOffset;
		}
	}

	if (m_HeadOffset == m_TailOffset)
		m_HeadOffset = GetOldestTailMemOffset();

	if (m_TailOffset < m_HeadOffset) {
		m_FreeMemory = m_HeadOffset - m_TailOffset;

		if (MemAvailable(memsize)) {
			freeBlockStart = m_TailOffset;
			m_TailOffset += memsize;
			m_FreeMemory -= memsize;
			return freeBlockStart;
		}
		else {
			//advance head
			m_HeadOffset = GetOldestTailMemOffset();
		}
	}	

	return FreeUpMem(memsize);
}

UINT64 roGraphicsRingBuffer::GetOldestTailMemOffset() {
	if (m_TailFrames.empty()) {
		THROW(L"Ring buffer not enough memory, consider large mem allocation with factor of frame buffer count")
	}

	auto oldTailFrame = m_TailFrames.front();
	uint64_t fenceValue = oldTailFrame.FenceValue;

	roCommandQueue::WaitForFenceValue(fenceValue);
	UINT64 oldTailOffset = oldTailFrame.BufferOffset;
	m_TailFrames.pop_front();

	return oldTailOffset;
}