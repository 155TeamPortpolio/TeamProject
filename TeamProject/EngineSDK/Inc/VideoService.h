#pragma once
#include "Base.h"
#include "ThreadPool.h"
#include "VideoPlayer.h"
#include "IVideoDecoderBackend.h"

class ENGINE_DLL CVideoService : public CBase
{
private:
    CVideoService();
    ~CVideoService() DEFAULT;

public:
    struct VIDEO_PLAYER_CONTEXT
    {
        IVideoDecoderBackend* pDecoder =    { nullptr };
        atomic<_bool> cancelRequested  =    { false   };
    };

public:
    void TickPresent(_uint64 nowPts);
    void Tick(_float deltaSeconds);

public:
    _bool Initialize(class CThreadPool* threadPool, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    _uint  CreatePlayer(IVideoDecoderBackend* pDecoderBackend);
    CVideoPlayer* GetPlayer(_uint playerId);
    void DestroyPlayer(_uint playerId);
    void StartDecode(_uint playerId);
private:
    void DecodeLoop(_uint playerId);



private:
    mutex m_mutex;
    CThreadPool* m_threadPool = { nullptr };
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    unordered_map<_uint, CVideoPlayer*> m_VideoPlayers;
    unordered_map<_uint, VIDEO_PLAYER_CONTEXT> m_VideoContexts;
    atomic<_uint> m_nextPlayerId{ 1 };
    _bool m_mfStarted = { false };
public:
    static CVideoService* Create(class CThreadPool* threadPool, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free();
};
