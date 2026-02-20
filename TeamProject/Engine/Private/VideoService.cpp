#include "Engine_Defines.h"
#include "VideoService.h"
#include "ThreadPool.h"
#include <mfapi.h>
#include "VideoPlayer.h"

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

    playerObject->SetID(newPlayerID);
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
    CVideoPlayer* playerToRelease = nullptr;
    IVideoDecoderBackend* decoderToClose = nullptr;

    {
        std::lock_guard<std::mutex> lockGuard(m_mutex);

        auto contextIterator = m_VideoContexts.find(playerId);
        if (contextIterator == m_VideoContexts.end())
            return;

        VIDEO_PLAYER_CONTEXT& context = contextIterator->second;

        context.cancelRequested.store(true, std::memory_order_release);
        context.pendingDestroy.store(true, std::memory_order_release);

        if (context.pDecoder)
            context.pDecoder->RequestStopDecode(); // stop=true + flush 로 구현되어 있어야 함

        // ? “이미 돌고 있거나”, “enqueue만 된 상태”면 여기서 해제 금지
        if (context.decodeRunning.load(std::memory_order_acquire) ||
            context.decodeRequested.load(std::memory_order_acquire))
            return;

        // 여기 도달 = 루프도 없고 예정도 없음 -> 지금 바로 정리 가능
        decoderToClose = context.pDecoder;

        auto playerIterator = m_VideoPlayers.find(playerId);
        if (playerIterator != m_VideoPlayers.end())
            playerToRelease = playerIterator->second;

        m_VideoPlayers.erase(playerId);
        m_VideoContexts.erase(playerId);
    }

    if (decoderToClose) decoderToClose->Close();
    if (playerToRelease) Safe_Release(playerToRelease);
}
void CVideoService::StartDecode(_uint playerId)
{
    CVideoPlayer* playerInstance = nullptr;
    IVideoDecoderBackend* decoderBackend = nullptr;
    CVideoPlayer::VIDEO_PLAYER_DESC descCopy{};

    {
        std::lock_guard<std::mutex> lockGuard(m_mutex);

        auto playerIterator = m_VideoPlayers.find(playerId);
        auto contextIterator = m_VideoContexts.find(playerId);
        if (playerIterator == m_VideoPlayers.end() || contextIterator == m_VideoContexts.end())
            return;

        VIDEO_PLAYER_CONTEXT& context = contextIterator->second;

        // 플래그 중복 방지
        if (context.decodeRunning.load(std::memory_order_acquire) ||
            context.decodeRequested.exchange(true, std::memory_order_acq_rel))
            return;

        context.cancelRequested.store(false, std::memory_order_release);

        playerInstance = playerIterator->second;
        decoderBackend = context.pDecoder;

        // ★ 여기서 null이면 “contextPtr->...”에서 바로 터지는 원인
        if (!playerInstance || !decoderBackend)
        {
            context.decodeRequested.store(false, std::memory_order_release);
            return;
        }

        // 락 안에서 desc를 값으로 복사 (GetDesc()가 ref 리턴이라서)
        descCopy = playerInstance->GetDesc();

        // CBase 계열이면 안전하게 수명 확보 (Safe_AddRef 지원 가정)
        Safe_AddRef(playerInstance);
    }

    // --- 락 밖에서 작업 ---
    // stop 플래그 관련 함수 이름/의미는 네 구현에 맞게 하나만 호출
    // decoderBackend->ResetStopDecode();  // (네가 유지하겠다면)
    // decoderBackend->RequestStopDecode(); // 이런 식은 여기서 호출하면 안 맞음

    if (!decoderBackend->Open(descCopy.filePath))
    {
        playerInstance->Stop();

        std::lock_guard<std::mutex> lockGuard(m_mutex);
        auto contextIterator = m_VideoContexts.find(playerId);
        if (contextIterator != m_VideoContexts.end())
            contextIterator->second.decodeRequested.store(false, std::memory_order_release);

        Safe_Release(playerInstance);
        return;
    }

    decoderBackend->SetLoop(descCopy.loop);
    m_threadPool->enqueue([this, playerId] { DecodeLoop(playerId); });

    Safe_Release(playerInstance);
}
IVideoDecoderBackend* CVideoService::Get_OwnDecoder(_uint playerId)
{
    lock_guard<mutex> lockGuard(m_mutex);

    auto contextIterator = m_VideoContexts.find(playerId);
    if (contextIterator == m_VideoContexts.end())
        return nullptr;

    return contextIterator->second.pDecoder;
}
void CVideoService::DecodeLoop(_uint playerId)
{
    CVideoPlayer* playerPtr = nullptr;
    VIDEO_PLAYER_CONTEXT* contextPtr = nullptr;

    {
        lock_guard<mutex> lockGuard(m_mutex);

        auto playerIterator = m_VideoPlayers.find(playerId);
        auto contextIterator = m_VideoContexts.find(playerId);
        if (playerIterator == m_VideoPlayers.end() || contextIterator == m_VideoContexts.end())
            return;

        playerPtr = playerIterator->second;
        contextPtr = &contextIterator->second;

        contextPtr->decodeRunning.store(true, std::memory_order_release);
    }

    auto FinishFlags = [&]()
        {
            CVideoPlayer* playerToRelease = nullptr;
            IVideoDecoderBackend* decoderToClose = nullptr;

            {
                std::lock_guard<std::mutex> lockGuard(m_mutex);
                auto contextIterator = m_VideoContexts.find(playerId);
                if (contextIterator == m_VideoContexts.end())
                    return;

                VIDEO_PLAYER_CONTEXT& context = contextIterator->second;
                context.decodeRunning.store(false, std::memory_order_release);
                context.decodeRequested.store(false, std::memory_order_release);

                if (!context.pendingDestroy.load(std::memory_order_acquire))
                    return;

                decoderToClose = context.pDecoder;

                auto playerIterator = m_VideoPlayers.find(playerId);
                if (playerIterator != m_VideoPlayers.end())
                    playerToRelease = playerIterator->second;

                m_VideoPlayers.erase(playerId);
                m_VideoContexts.erase(playerId);
            }

            if (decoderToClose) decoderToClose->Close();
            if (playerToRelease) Safe_Release(playerToRelease);
        };
    while (true)
    {
        // ====== (1) 컨텍스트 유효/취소 확인 ======
        {
            bool shouldExit = false;

            {
                lock_guard<mutex> lockGuard(m_mutex);

                auto playerIterator = m_VideoPlayers.find(playerId);
                auto contextIterator = m_VideoContexts.find(playerId);

                if (playerIterator == m_VideoPlayers.end() || contextIterator == m_VideoContexts.end())
                {
                    shouldExit = true;
                }
                else
                {
                    playerPtr = playerIterator->second;
                    contextPtr = &contextIterator->second;

                    if (contextPtr->cancelRequested.load(std::memory_order_acquire))
                        shouldExit = true;
                }
            }

            if (shouldExit)
            {
                FinishFlags();
                return;
            }
        }

        // ====== (2) 재생 상태/큐 상태 ======
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

        // ====== (3) 디코드 ======
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
            FinishFlags();
            return;
        }

        if (!decoded)
        {
            this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        // ====== (4) 프레임 푸시 ======
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
    vector<pair<_uint, CVideoPlayer*>> players;
    players.reserve(m_VideoPlayers.size());

    {
        lock_guard<mutex> lockGuard(m_mutex);
        for (auto& pair : m_VideoPlayers)
            players.push_back(pair);
    }

    for (auto& pair : players)
    {
        const _uint playerId = pair.first;
        CVideoPlayer* playerPtr = pair.second;
        if (!playerPtr) continue;

        // replay 처리에서만 컨텍스트를 다시 서비스 락으로 조회
        if (playerPtr->ConsumeReplayRequest())
        {
            IVideoDecoderBackend* decoderPtr = nullptr;

            {
                lock_guard<mutex> lockGuard(m_mutex);
                auto ctxIt = m_VideoContexts.find(playerId);
                if (ctxIt != m_VideoContexts.end())
                    decoderPtr = ctxIt->second.pDecoder;
            }

            if (decoderPtr)
                decoderPtr->SeekSeconds(0.0f);

            playerPtr->Replay();
        }

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
