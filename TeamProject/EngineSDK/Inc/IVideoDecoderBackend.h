#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL IVideoDecoderBackend : public CBase
{
public:
    virtual ~IVideoDecoderBackend() = default;
public:
    virtual _bool Open(const string& filePath)  PURE;
    virtual _bool ReOpen()                      PURE;
    virtual void Close()                        PURE;
    virtual void SetLoop(_bool loop)            PURE;
    virtual _bool DecodeNextRGBA(
        vector<uint8_t>& outRgba,
        _uint& outWidth,
        _uint& outHeight,
        _uint64& outPts,
        _bool& outEnded)                        PURE;
    virtual _bool SeekSeconds(_float seconds)    PURE;
public:
    virtual void RequestStopDecode()PURE;   // 중지 요청 + ReadSample 깨우기 시도
    virtual void ResetStopDecode()PURE;     // 재생 시작 전에 플래그 초기화
    bool IsStopRequested() const { return m_stopRequested.load(std::memory_order_acquire); }

protected:
   atomic_bool m_stopRequested = false;
   mutex m_readerMutex; // m_reader 접근 동기화 (Flush/Close vs DecodeNextRGBA)
};
NS_END