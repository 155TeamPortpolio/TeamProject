#pragma once
#include "IVideoDecoderBackend.h"
NS_BEGIN(Client)
class CDecoder :
    public IVideoDecoderBackend
{
public:
    CDecoder();
    virtual ~CDecoder() DEFAULT;

public:
    bool Open(const string& filePath) override;
    void Close() override;
    void SetLoop(bool loop) override;
    _bool SeekSeconds(float seconds) override;
    bool DecodeNextRGBA(
        vector<uint8_t>& outRgba,
        _uint& outWidth,
        _uint& outHeight,
        _uint64& outPts,
        _bool& outEnded) override;

private:
    _uint64 GetFrameDurationMs() const;

private:
    _bool m_isOpened = { false };
    _bool m_isLoop = { false };

    _uint m_width = { 0 };
    _uint m_height = { 0 };
    _uint m_fpsNumerator = { 30 };
    _uint m_fpsDenominator = { 1 };
    _uint64 m_frameIndex = { 0 };
    _uint64 m_durationMs = { 0 };

    chrono::steady_clock::time_point m_startClock{};

public:
    static CDecoder* Create();
};
NS_END