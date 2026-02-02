#include "Engine_Defines.h"
#include "VideoPlayer.h"
#include "IVideoDecoderBackend.h"

CVideoPlayer::CVideoPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice(pDevice)
    , m_pContext(pContext)
    , m_frameQueue(4)
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

bool CVideoPlayer::Open(const VIDEO_PLAYER_DESC& desc)
{
    lock_guard<mutex> lock(m_mutex);

    m_desc = desc;
    m_playPtsMs = 0;
    m_state.store(VIDEO_PLAY_STATE::Ready, std::memory_order_release);
    m_frameQueue.Clear();
    return true;
}

void CVideoPlayer::Close()
{
    {
        lock_guard<mutex> lock(m_mutex);
        m_state.store(VIDEO_PLAY_STATE::Closed, std::memory_order_release);
        m_frameQueue.Clear();
        m_playPtsMs = 0;
    }

    Safe_Release(m_pSRV);
    Safe_Release(m_pTexture);

    m_width = 0;
    m_height = 0;
}

void CVideoPlayer::Play()
{
    const VIDEO_PLAY_STATE prev = m_state.load(std::memory_order_acquire);

    if (prev == VIDEO_PLAY_STATE::Ended)
    {
        RequestReplay();
        return;
    }

    m_state.store(VIDEO_PLAY_STATE::Playing, std::memory_order_release);
}

void CVideoPlayer::Pause()
{
    m_state.store(VIDEO_PLAY_STATE::Paused, std::memory_order_release);
}

void CVideoPlayer::Stop()
{
    lock_guard<mutex> lock(m_mutex);
    m_state.store(VIDEO_PLAY_STATE::Ended, std::memory_order_release);
    m_frameQueue.Clear();
    m_playPtsMs = 0;
}

void CVideoPlayer::PushDecodedFrame(VIDEO_FRAME_CPU&& frame)
{
    if (frame.width == 0 || frame.height == 0)
        return;

    m_pushCount.fetch_add(1, std::memory_order_relaxed);

    lock_guard<mutex> lock(m_mutex);
    m_frameQueue.PushDropOldest(std::move(frame));
}

void CVideoPlayer::PumpPresent(_uint64 nowPts)
{
    if (m_state.load(std::memory_order_acquire) != VIDEO_PLAY_STATE::Playing)
        return;

    VIDEO_FRAME_CPU pickedFrame;

    {
        lock_guard<mutex> lock(m_mutex);

        if (!m_frameQueue.PopLatestNotAfter(nowPts, pickedFrame))
            return;
    }

    m_presentCount.fetch_add(1, std::memory_order_relaxed);

    if (!EnsureGpuTexture(pickedFrame.width, pickedFrame.height))
        return;

    const UINT rowPitch = (UINT)pickedFrame.width * 4;
    m_pContext->UpdateSubresource(m_pTexture, 0, nullptr,
        pickedFrame.rgba.data(), rowPitch, 0);
}

_bool CVideoPlayer::EnsureGpuTexture(_uint width, _uint height)
{
    if (m_pTexture && m_pSRV && m_width == width && m_height == height)
        return true;

    Safe_Release(m_pTexture);
    Safe_Release(m_pSRV);

    m_width = width;
    m_height = height;

    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;

    // CPU 버퍼가 BGRA라고 가정 (MF RGB32도 보통 BGRA)
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_TYPELESS;

    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    HRESULT hrTexture = m_pDevice->CreateTexture2D(&textureDesc, nullptr, &m_pTexture);
    if (FAILED(hrTexture))
        return false;

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    HRESULT hrSrv = m_pDevice->CreateShaderResourceView(m_pTexture, &srvDesc, &m_pSRV);
    if (FAILED(hrSrv))
        return false;

    return true;
}

void CVideoPlayer::SkipToEnd()
{
    lock_guard<mutex> lock(m_mutex);
    m_frameQueue.Clear();
    m_state.store(VIDEO_PLAY_STATE::Ended, std::memory_order_release);
}

void CVideoPlayer::Replay()
{
    lock_guard<mutex> lock(m_mutex);

    m_playPtsMs = 0;
    m_frameQueue.Clear();

    // 재생 시작
    m_state.store(VIDEO_PLAY_STATE::Playing, std::memory_order_release);
}

void CVideoPlayer::SetDecoder(IVideoDecoderBackend* decoder)
{
    lock_guard<mutex> lock(m_mutex);
    m_decoder = decoder;
}

//void CVideoPlayer::PumpPresent(_uint64 nowPts)
//{
//    if (GetState() != VIDEO_PLAY_STATE::Playing)
//        return;
//
//    VIDEO_FRAME_CPU pickedFrame;
//
//    if (!m_frameQueue.PopLatestNotAfterOrClosest(nowPts, pickedFrame))
//        return;
//
//    if (!EnsureGpuTexture(pickedFrame.width, pickedFrame.height))
//    {
//        m_state.store(VIDEO_PLAY_STATE::Error, std::memory_order_release);
//        return;
//    }
//
//    const UINT rowPitch = (UINT)pickedFrame.width * 4;
//    m_pContext->UpdateSubresource(m_pTexture, 0, nullptr, pickedFrame.rgba.data(), rowPitch, 0);
//}

const CVideoPlayer::VIDEO_PLAYER_DESC& CVideoPlayer::GetDesc() const
{
    return m_desc;
}

_bool CVideoPlayer::IsQueueFullSoft()
{
    return m_frameQueue.Size() >= 4; // 네 정책대로
}


ID3D11ShaderResourceView* CVideoPlayer::GetSRV() const 
{
    return m_pSRV;
};
VIDEO_PLAY_STATE CVideoPlayer::GetState() const
{
    return m_state.load(std::memory_order_acquire);
};
bool CVideoPlayer::IsPlaying() const
{
    return GetState() == VIDEO_PLAY_STATE::Playing;
};
_uint CVideoPlayer::GetWidth() const
{
    return m_width;
};
_uint CVideoPlayer::GetHeight() const
{
    return m_height;
};

void CVideoPlayer::AdvanceClock(_float dt)
{
    if (m_state.load(std::memory_order_acquire) != VIDEO_PLAY_STATE::Playing)
        return;

    if (dt < 0.f) dt = 0.f;
    const float clampedDt = (dt > 0.1f) ? 0.1f : dt;

    lock_guard<mutex> lock(m_mutex);
    m_playPtsMs += (uint64_t)(clampedDt * 1000.0f + 0.5f);
}

uint64_t CVideoPlayer::GetClockMs() const
{
    return m_playPtsMs;
}


CVideoPlayer* CVideoPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    return new CVideoPlayer(pDevice, pContext);
}

void CVideoPlayer::Free()
{
    Safe_Release(m_pSRV);
    Safe_Release(m_pTexture);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
