#pragma once
#include "Engine_Defines.h"
#include "Base.h"
#include "FrameQueue.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVideoPlayer : public CBase
{
public:
    struct VIDEO_PLAYER_DESC
    {
        string filePath;
        _bool loop = false;
        _float volume = 1.f;
    };

private:
    CVideoPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    ~CVideoPlayer() DEFAULT;

public:
    bool Open(const VIDEO_PLAYER_DESC& desc);
    void Close();

    void Play();
    void Pause();
    void Stop();
    void SkipToEnd();

public:
    ID3D11ShaderResourceView* GetSRV() const;
    VIDEO_PLAY_STATE GetState() const;
    const VIDEO_PLAYER_DESC& GetDesc() const;
    bool IsPlaying() const;

    _uint GetWidth() const;
    _uint GetHeight() const;

public:
    void PumpPresent(_uint64 nowPts);
    void PushDecodedFrame(VIDEO_FRAME_CPU&& frame);
    _bool IsQueueFullSoft() ;
    void AdvanceClock(_float dt);
    uint64_t GetClockMs() const;

private:
    _bool EnsureGpuTexture(_uint width, _uint height);

private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;
    ID3D11Texture2D* m_pTexture = nullptr;
    ID3D11ShaderResourceView* m_pSRV = nullptr;

    VIDEO_PLAYER_DESC m_desc = {};
    atomic<VIDEO_PLAY_STATE> m_state{ VIDEO_PLAY_STATE::Closed };

    CFrameQueue m_frameQueue; // ★ 헤더에 {4} 금지

    _uint m_width = 0;
    _uint m_height = 0;

public:
    atomic<uint64_t> m_pushCount{ 0 };
    atomic<uint64_t> m_presentCount{ 0 };
    uint64_t m_playPtsMs = 0;

public:
    static CVideoPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free() override;
};

NS_END
