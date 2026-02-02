#pragma once
#include "IVideoDecoderBackend.h"

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <wrl/client.h>
#include <cstdint>

NS_BEGIN(Engine)
class ENGINE_DLL CMFVideoDecoderBackend final : public IVideoDecoderBackend
{
public:
    CMFVideoDecoderBackend();
    ~CMFVideoDecoderBackend() DEFAULT;

public:
    bool Open(const std::string& filePath) override;
    void Close() override;

    void SetLoop(bool loop) override;
    _bool SeekSeconds(float seconds) override;

    bool DecodeNextRGBA(
        std::vector<std::uint8_t>& outRgba,
        std::uint32_t& outWidth,
        std::uint32_t& outHeight,
        std::uint64_t& outPtsMs,
        bool& outEnded) override;

public:

private:
    bool ConfigureOutputToRGB32();   // 성공하면 m_outputSubtype = RGB32
    bool ConfigureOutputToNV12();    // 성공하면 m_outputSubtype = NV12
    bool QuerySizeFromCurrentType(); // m_width/m_height/m_stride 세팅

private:
    Microsoft::WRL::ComPtr<IMFSourceReader> m_reader;

    bool m_isOpened = false;
    bool m_isLoop = false;

    std::uint32_t m_width = 0;
    std::uint32_t m_height = 0;

    // RGB32일 때 stride가 width*4가 아닐 수 있음 (행 단위 복사용)
    LONG m_strideBytes = 0;

    // 현재 SourceReader 출력 서브타입 (NV12 / RGB32)
    GUID m_outputSubtype = GUID_NULL;

    // duration(100ns), 선택
    std::int64_t m_durationHns = -1;
    uint64_t m_firstPtsMs = UINT64_MAX;

public:
    static CMFVideoDecoderBackend* Create() { return new CMFVideoDecoderBackend(); }
    virtual void Free() override;
};
NS_END