#include "Engine_Defines.h"
#include "FrameQueue.h"

CFrameQueue::CFrameQueue(_uint capacity)
	: m_capacity(capacity)
{
}

void CFrameQueue::Clear()
{
	lock_guard<mutex> guard(m_mutex);
	m_queue.clear();
}

_uint CFrameQueue::Size()
{
	lock_guard<mutex> guard(m_mutex);
	return (_uint)m_queue.size();
}
void CFrameQueue::PushDropOldest(VIDEO_FRAME_CPU&& frame)
{
	lock_guard<mutex> guard(m_mutex);
	if (m_queue.size() >= m_capacity)
		m_queue.pop_front();
	m_queue.emplace_back(move(frame));
}

uint64_t CFrameQueue::GetPtsMs(const VIDEO_FRAME_CPU& frame)
{
    return (std::uint64_t)frame.PresentTime;
}

bool CFrameQueue::PopLatestNotAfter(uint64_t nowPts, VIDEO_FRAME_CPU& outFrame)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_queue.empty())
        return false;

    // 1) nowPts 이하 중 가장 최신 찾기
    int pickedIndex = -1;
    for (int index = (int)m_queue.size() - 1; index >= 0; --index)
    {
        const uint64_t framePts = GetPtsMs(m_queue[(size_t)index]);
        if (framePts <= nowPts)
        {
            pickedIndex = index;
            break;
        }
    }

    if (pickedIndex >= 0)
    {
        outFrame = std::move(m_queue[(size_t)pickedIndex]);
        m_queue.erase(m_queue.begin(), m_queue.begin() + (size_t)pickedIndex + 1);
        return true;
    }

    // 2) 여기까지 왔다는 건 "전부 nowPts보다 미래"라는 뜻.
    //    이 경우 첫 프레임(가장 이른 미래)을 하나 뽑아서 진행을 깨뜨리지 않게 함.
    outFrame = std::move(m_queue.front());
    m_queue.pop_front();
    return true;
}
// FrameQueue.h 내부
bool CFrameQueue::PopLatestNotAfterOrClosest(uint64_t nowPts, VIDEO_FRAME_CPU& outFrame)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_queue.empty())
        return false;

    // nowPts 이하 중 가장 최신 찾기
    int pickedIndex = -1;
    for (int index = (int)m_queue.size() - 1; index >= 0; --index)
    {
        const uint64_t framePts = (uint64_t)m_queue[(size_t)index].PresentTime;
        if (framePts <= nowPts)
        {
            pickedIndex = index;
            break;
        }
    }

    if (pickedIndex >= 0)
    {
        outFrame = std::move(m_queue[(size_t)pickedIndex]);
        m_queue.erase(m_queue.begin(), m_queue.begin() + (size_t)pickedIndex + 1);
        return true;
    }

    // 전부 미래면: 가장 이른 프레임 1개라도 뽑아서 진행
    outFrame = std::move(m_queue.front());
    m_queue.pop_front();
    return true;
}

bool CFrameQueue::PeekFrontBackPts(uint64_t& outFront, uint64_t& outBack, size_t& outSize) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    outSize = m_queue.size();
    if (m_queue.empty())
        return false;

    outFront = (uint64_t)m_queue.front().PresentTime;
    outBack = (uint64_t)m_queue.back().PresentTime;
    return true;
}
