#include "pch.h"
#include "Decoder.h"

 CDecoder::CDecoder()
{
}

_bool CDecoder::Open(const string& filePath)
{
    return false;
}

void CDecoder::Close()
{
}

void CDecoder::SetLoop(bool loop)
{
}

 void CDecoder::SeekSeconds(float seconds)
{
}

_bool CDecoder::DecodeNextRGBA(vector<uint8_t>& outRgba, _uint& outWidth, _uint& outHeight, _uint64& outPts, _bool& outEnded)
{
    return false;
}

_uint64 CDecoder::GetFrameDurationMs() const
{
    const double fps = (double)m_fpsNumerator / (double)m_fpsDenominator;
    const double frameMs = 1000.0 / fps;
    return (uint64_t)(frameMs + 0.5);
}
