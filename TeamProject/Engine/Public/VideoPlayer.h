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
    enum class VIDEO_CMD : uint8_t
    {
        None = 0,
        Replay = 1,
    };

    atomic<VIDEO_CMD> m_cmd{ VIDEO_CMD::None };

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
    void Replay();
    void SetDecoder(class IVideoDecoderBackend* decoder);
    void RequestReplay(){ m_replayRequested.store(true, std::memory_order_release);}
    bool ConsumeReplayRequest(){return m_replayRequested.exchange(false, std::memory_order_acq_rel); }

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
    void SetID(_uint ID) { m_ID; }

private:
    _bool EnsureGpuTexture(_uint width, _uint height);
    VIDEO_CMD ConsumeCmd()
    {
        return m_cmd.exchange(VIDEO_CMD::None, std::memory_order_acq_rel);
    }

private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;
    ID3D11Texture2D* m_pTexture = nullptr;
    ID3D11ShaderResourceView* m_pSRV = nullptr;

    VIDEO_PLAYER_DESC m_desc = {};
    atomic<VIDEO_PLAY_STATE> m_state{ VIDEO_PLAY_STATE::Closed };
  
    mutex m_mutex;
    CFrameQueue m_frameQueue; 
    class IVideoDecoderBackend* m_decoder = nullptr; 

    _uint m_width = 0;
    _uint m_height = 0;
    _uint m_ID = {};
public:
    atomic<uint64_t> m_pushCount{ 0 };
    atomic<uint64_t> m_presentCount{ 0 };
    atomic<bool> m_replayRequested{ false };
    uint64_t m_playPtsMs = 0;

public:
    static CVideoPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual void Free() override;
};

NS_END
