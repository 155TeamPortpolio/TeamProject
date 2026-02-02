#pragma once
NS_BEGIN(Engine)
struct VIDEO_FRAME_CPU {
	_uint64 PresentTime = { 0 };
	_uint width = { 0 };
	_uint height = { 0 };
	vector<uint8_t> rgba;
};

class CFrameQueue
{
public:
	explicit CFrameQueue(_uint capacity);

public:
	_uint Size();
	void Clear();
	void PushDropOldest(VIDEO_FRAME_CPU&& frame);
	_bool PopLatestNotAfter(_uint64 nowPresent, VIDEO_FRAME_CPU& outFrame);
	_bool PopOldest(VIDEO_FRAME_CPU& out);
	uint64_t GetPtsMs(const VIDEO_FRAME_CPU& frame);
	bool PopLatestNotAfterOrClosest(uint64_t nowPts, VIDEO_FRAME_CPU& outFrame);
	bool PeekFrontBackPts(uint64_t& outFront, uint64_t& outBack, size_t& outSize) const;

private:
	_uint m_capacity = 0;
	mutable mutex m_mutex;
	deque<VIDEO_FRAME_CPU> m_queue;
};
NS_END

