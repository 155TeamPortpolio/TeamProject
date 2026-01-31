#include "Engine_Defines.h"
#include "VideoService.h"
#include "ThreadPool.h"
#include <mfapi.h>

CVideoService::CVideoService()
{
}

_bool CVideoService::Initialize(CThreadPool* threadPool, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    if(!threadPool || !pDevice || !pContext)
        return false;

    m_threadPool = threadPool;
    m_pDevice = pDevice;
    m_pContext = pContext;

    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_threadPool);

    HRESULT hr = MFStartup(MF_VERSION);
    if (FAILED(hr))
        return false;

    m_mfStarted = true;

    return true;
}

_uint CVideoService::CreatePlayer(IVideoDecoderBackend* pDecoderBackend)
{
    if(!pDecoderBackend)
        return 0;
    const _uint newPlayerID = m_nextPlayerId.fetch_add(1, memory_order_acq_rel);
    auto playerObject = CVideoPlayer::Create(m_pDevice, m_pContext);

    {
        lock_guard<mutex> lockGuard(m_mutex);

        m_VideoPlayers.emplace(newPlayerID, std::move(playerObject));

        auto [iter, sec] = m_VideoContexts.try_emplace(newPlayerID); // in-place 기본 생성
        VIDEO_PLAYER_CONTEXT& context = iter->second;
        context.pDecoder = pDecoderBackend;
        context.cancelRequested.store(false, std::memory_order_release);
    }


    return newPlayerID;
}

CVideoPlayer* CVideoService::GetPlayer(_uint playerId)
{
    lock_guard<mutex> lockGuard(m_mutex);
    auto iterator = m_VideoPlayers.find(playerId);
    if (iterator == m_VideoPlayers.end())
        return nullptr;
    return iterator->second;
}

void CVideoService::DestroyPlayer(_uint playerId)
{
    lock_guard<mutex> lockGuard(m_mutex);

    auto contextIterator = m_VideoContexts.find(playerId);
    if (contextIterator != m_VideoContexts.end())
    {
        contextIterator->second.cancelRequested.store(true, std::memory_order_release);
        if (contextIterator->second.pDecoder)
            contextIterator->second.pDecoder->Close(); 
    }

    auto iter = m_VideoPlayers.find(playerId);
    if (iter != m_VideoPlayers.end())
        Safe_Release(iter->second);

    m_VideoPlayers.erase(playerId);
    m_VideoContexts.erase(playerId);
}

void CVideoService::StartDecode(_uint playerId)
{

    VIDEO_PLAYER_CONTEXT* contextPtr = nullptr;
    CVideoPlayer* playerPtr = nullptr;

    {
        lock_guard<mutex> lockGuard(m_mutex);

        auto playerIterator = m_VideoPlayers.find(playerId);
        auto contextIterator = m_VideoContexts.find(playerId);
        if (playerIterator == m_VideoPlayers.end() || contextIterator == m_VideoContexts.end())
            return;

        playerPtr = playerIterator->second;
        contextPtr = &contextIterator->second;

        contextPtr->cancelRequested.store(false, memory_order_release);
    }
    const auto& desc = playerPtr->GetDesc();

    if (!contextPtr->pDecoder->Open(desc.filePath))
    {
        playerPtr->Stop(); // 혹은 Error 상태로
        return;
    }

    contextPtr->pDecoder->SetLoop(desc.loop);
    m_threadPool->enqueue([this, playerId] { DecodeLoop(playerId); });
}

void CVideoService::DecodeLoop(_uint playerId)
{
    while (true)
    {
        CVideoPlayer* playerPtr = nullptr;
        VIDEO_PLAYER_CONTEXT* contextPtr = nullptr;
        {
            lock_guard<mutex> lockGuard(m_mutex);

            auto playerIterator = m_VideoPlayers.find(playerId);
            auto contextIterator = m_VideoContexts.find(playerId);
            if (playerIterator == m_VideoPlayers.end() || contextIterator == m_VideoContexts.end())
                return;

            playerPtr  =     playerIterator->second;
            contextPtr =    &contextIterator->second;
        }

        if (contextPtr->cancelRequested.load(memory_order_acquire))
            return;

        if (!playerPtr->IsPlaying())
        {
            this_thread::sleep_for(chrono::milliseconds(2));
            continue;
        }

        if (playerPtr->IsQueueFullSoft())
        {
            this_thread::sleep_for(chrono::milliseconds(1));
            continue;
        }

        _bool ended = false;
        _uint decodedWidth = 0;
        _uint decodedHeight = 0;
        _uint64 decodedPts = 0;
        vector<uint8_t> rgbaBytes;

        bool decoded = contextPtr->pDecoder->DecodeNextRGBA(
            rgbaBytes, decodedWidth, decodedHeight, decodedPts, ended);

        if (ended)
        {
            playerPtr->Stop();
            return;
        }

        if (!decoded)
        {
            // 일시적으로 못 뽑는 상황이면 살짝 쉬고 재시도
            this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        VIDEO_FRAME_CPU frame;
        frame.PresentTime = decodedPts;
        frame.width = decodedWidth;
        frame.height = decodedHeight;
        frame.rgba = std::move(rgbaBytes);

        playerPtr->PushDecodedFrame(std::move(frame));
    }
}

void CVideoService::Tick(_float dt)
{
    lock_guard<mutex> lockGuard(m_mutex);
    for (auto& pair : m_VideoPlayers)
    {
        CVideoPlayer* playerPtr = pair.second;
        if (!playerPtr) continue;

        playerPtr->AdvanceClock(dt);
        playerPtr->PumpPresent(playerPtr->GetClockMs());
    }
}

void CVideoService::TickPresent(_uint64 nowPts)
{
    lock_guard<mutex> lockGuard(m_mutex);
    for (auto& pair : m_VideoPlayers)
    {
        CVideoPlayer* playerPtr = pair.second;
        if (playerPtr)
            playerPtr->PumpPresent(nowPts);
    }
}

CVideoService* CVideoService::Create(class CThreadPool* threadPool, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CVideoService* instance = new CVideoService();
    if (FAILED(instance->Initialize(threadPool, pDevice, pContext))) {
        Safe_Release(instance);
    }
    return instance;
}

void CVideoService::Free() 
{
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_threadPool);
    if (m_mfStarted)
    {
        MFShutdown();
        m_mfStarted = false;
    }
    for (auto player : m_VideoPlayers)
    {
        Safe_Release(player.second);
    }
}
