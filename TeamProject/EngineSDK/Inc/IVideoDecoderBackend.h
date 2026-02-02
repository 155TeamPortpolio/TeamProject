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
};
NS_END