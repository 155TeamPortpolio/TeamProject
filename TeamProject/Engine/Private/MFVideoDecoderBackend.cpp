#include "Engine_Defines.h"
#include "MFVideoDecoderBackend.h"

#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <propvarutil.h>
#include <wrl/client.h>

#include <cstdint>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

// ------------------------------------------------------------
// helpers
// ------------------------------------------------------------
static std::wstring ConvertUtf8ToWide(const std::string& utf8Text)
{
    if (utf8Text.empty())
        return std::wstring();

    int wideCountWithNull = MultiByteToWideChar(CP_UTF8, 0, utf8Text.c_str(), -1, nullptr, 0);
    if (wideCountWithNull <= 0)
        wideCountWithNull = MultiByteToWideChar(CP_ACP, 0, utf8Text.c_str(), -1, nullptr, 0);

    if (wideCountWithNull <= 0)
        return std::wstring();

    std::wstring wideText;
    wideText.resize((size_t)wideCountWithNull - 1);

    int written = MultiByteToWideChar(CP_UTF8, 0, utf8Text.c_str(), -1, wideText.data(), wideCountWithNull);
    if (written <= 0)
    {
        written = MultiByteToWideChar(CP_ACP, 0, utf8Text.c_str(), -1, wideText.data(), wideCountWithNull);
        if (written <= 0)
            return std::wstring();
    }

    return wideText;
}

static inline std::uint8_t ClampByte(int intValue)
{
    if (intValue < 0) return 0;
    if (intValue > 255) return 255;
    return (std::uint8_t)intValue;
}
static void ConvertNV12ToRGBA_WithStride(
    const std::uint8_t* nv12Data,
    std::uint32_t frameWidth,
    std::uint32_t frameHeight,
    std::uint32_t srcStrideBytes,
    std::vector<std::uint8_t>& outRgba)
{
    const std::uint8_t* yPlane = nv12Data;
    const std::uint8_t* uvPlane = nv12Data + (size_t)srcStrideBytes * (size_t)frameHeight;

    outRgba.resize((size_t)frameWidth * (size_t)frameHeight * 4);

    for (std::uint32_t rowIndex = 0; rowIndex < frameHeight; ++rowIndex)
    {
        const std::uint32_t yRowOffset = rowIndex * srcStrideBytes;
        const std::uint32_t uvRowOffset = (rowIndex / 2) * srcStrideBytes;

        for (std::uint32_t colIndex = 0; colIndex < frameWidth; ++colIndex)
        {
            const std::uint8_t yValue = yPlane[yRowOffset + colIndex];

            const std::uint32_t uvIndex = uvRowOffset + (colIndex / 2) * 2;
            const int uValue = (int)uvPlane[uvIndex + 0] - 128;
            const int vValue = (int)uvPlane[uvIndex + 1] - 128;

            const int cValue = (int)yValue - 16;
            const int dValue = uValue;
            const int eValue = vValue;

            int redValue = (298 * cValue + 409 * eValue + 128) >> 8;
            int greenValue = (298 * cValue - 100 * dValue - 208 * eValue + 128) >> 8;
            int blueValue = (298 * cValue + 516 * dValue + 128) >> 8;

            const size_t outIndex = ((size_t)rowIndex * (size_t)frameWidth + (size_t)colIndex) * 4;
            outRgba[outIndex + 0] = ClampByte(redValue);
            outRgba[outIndex + 1] = ClampByte(greenValue);
            outRgba[outIndex + 2] = ClampByte(blueValue);
            outRgba[outIndex + 3] = 255;
        }
    }
}
// NV12 -> RGBA (output order: R,G,B,A)
static void ConvertNV12ToRGBA(
    const std::uint8_t* nv12Data,
    std::uint32_t frameWidth,
    std::uint32_t frameHeight,
    std::vector<std::uint8_t>& outRgba)
{
    const std::uint8_t* yPlane = nv12Data;
    const std::uint8_t* uvPlane = nv12Data + (size_t)frameWidth * (size_t)frameHeight;

    outRgba.resize((size_t)frameWidth * (size_t)frameHeight * 4);

    for (std::uint32_t rowIndex = 0; rowIndex < frameHeight; ++rowIndex)
    {
        const std::uint32_t yRowOffset = rowIndex * frameWidth;
        const std::uint32_t uvRowOffset = (rowIndex / 2) * frameWidth;

        for (std::uint32_t colIndex = 0; colIndex < frameWidth; ++colIndex)
        {
            const std::uint8_t yValue = yPlane[yRowOffset + colIndex];

            const std::uint32_t uvIndex = uvRowOffset + (colIndex / 2) * 2;
            const int uValue = (int)uvPlane[uvIndex + 0] - 128;
            const int vValue = (int)uvPlane[uvIndex + 1] - 128;

            const int cValue = (int)yValue - 16;
            const int dValue = uValue;
            const int eValue = vValue;

            int redValue = (298 * cValue + 409 * eValue + 128) >> 8;
            int greenValue = (298 * cValue - 100 * dValue - 208 * eValue + 128) >> 8;
            int blueValue = (298 * cValue + 516 * dValue + 128) >> 8;

            const size_t outIndex = ((size_t)yRowOffset + (size_t)colIndex) * 4;
            outRgba[outIndex + 0] = ClampByte(redValue);
            outRgba[outIndex + 1] = ClampByte(greenValue);
            outRgba[outIndex + 2] = ClampByte(blueValue);
            outRgba[outIndex + 3] = 255;
        }
    }
}

struct ScopedBufferLock
{
    IMFMediaBuffer* buffer = nullptr;
    BYTE* dataPtr = nullptr;
    DWORD maxLen = 0;
    DWORD curLen = 0;

    bool Lock(IMFMediaBuffer* targetBuffer)
    {
        buffer = targetBuffer;
        if (!buffer) return false;

        HRESULT result = buffer->Lock(&dataPtr, &maxLen, &curLen);
        return SUCCEEDED(result);
    }

    ~ScopedBufferLock()
    {
        if (buffer)
            buffer->Unlock();
    }
};

// ------------------------------------------------------------
// CMFVideoDecoderBackend
// ------------------------------------------------------------
CMFVideoDecoderBackend::CMFVideoDecoderBackend()
{
}

void CMFVideoDecoderBackend::SetLoop(bool loop)
{
    m_isLoop = loop;
}
bool CMFVideoDecoderBackend::Open(const std::string& filePath)
{
    //  기존 디코드가 돌고 있으면 먼저 깨워서 빠지게
    RequestStopDecode();

    //  Close/Open/DecodeNextRGBA가 절대 겹치지 않게
    std::lock_guard<std::mutex> decodeGateLock(m_decodeGate);

    // 내부 상태 정리
    CloseInternal_NoDecodeGate();

    //  새 오픈은 stop 해제부터
    ResetStopDecode();

    std::wstring widePath = ConvertUtf8ToWide(filePath);
    if (widePath.empty())
        return false;

    m_lastFilePath = filePath;

    Microsoft::WRL::ComPtr<IMFAttributes> attributes;
    HRESULT result = MFCreateAttributes(attributes.GetAddressOf(), 8);
    if (FAILED(result))
        return false;

    attributes->SetUINT32(MF_READWRITE_DISABLE_CONVERTERS, FALSE);
    attributes->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, TRUE);
    attributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE);

    {
        std::lock_guard<std::mutex> readerLock(m_readerMutex);
        result = MFCreateSourceReaderFromURL(widePath.c_str(), attributes.Get(), m_reader.GetAddressOf());
    }
    if (FAILED(result))
        return false;

    result = m_reader->SetStreamSelection(MF_SOURCE_READER_FIRST_VIDEO_STREAM, TRUE);
    if (FAILED(result))
        return false;

    if (!ConfigureOutputToRGB32())
        return false;

    if (!QuerySizeFromCurrentType())
        return false;

    PROPVARIANT durationVar;
    PropVariantInit(&durationVar);
    result = m_reader->GetPresentationAttribute(MF_SOURCE_READER_MEDIASOURCE, MF_PD_DURATION, &durationVar);
    if (SUCCEEDED(result) && durationVar.vt == VT_UI8)
        m_durationHns = (std::int64_t)durationVar.uhVal.QuadPart;
    PropVariantClear(&durationVar);

    m_firstPtsMs = UINT64_MAX;
    m_isOpened = true;
    return true;
}

bool CMFVideoDecoderBackend::ReOpen()
{
    if (m_lastFilePath.empty())
        return false;

    Close();
    return Open(m_lastFilePath);
}
void CMFVideoDecoderBackend::Close()
{
    //  먼저 stop + flush로 ReadSample이 빨리 빠지게
    RequestStopDecode();

    //  DecodeNextRGBA가 진행 중이면 여기서 기다림
    std::lock_guard<std::mutex> decodeGateLock(m_decodeGate);

    CloseInternal_NoDecodeGate();
}

void CMFVideoDecoderBackend::CloseInternal_NoDecodeGate()
{
    {
        std::lock_guard<std::mutex> readerLock(m_readerMutex);
        m_reader.Reset();
    }

    m_isOpened = false;
    m_firstPtsMs = UINT64_MAX;
    m_width = 0;
    m_height = 0;
    m_strideBytes = 0;
    m_outputSubtype = GUID_NULL;
    m_durationHns = -1;

    // stop flag는 Open에서 Reset 해주는 쪽이 안전
}
void CMFVideoDecoderBackend::Free()
{
    Close();
}

void CMFVideoDecoderBackend::RequestStopDecode()
{
    m_stopRequested.store(true, std::memory_order_release);

    std::lock_guard<std::mutex> lock(m_readerMutex);
    if (m_reader)
    {
        m_reader->Flush(MF_SOURCE_READER_FIRST_VIDEO_STREAM);
    }
}

void CMFVideoDecoderBackend::ResetStopDecode()
{
    m_stopRequested.store(false, std::memory_order_release);
}

bool CMFVideoDecoderBackend::ConfigureOutputToRGB32()
{
    if (!m_reader)
        return false;

    Microsoft::WRL::ComPtr<IMFMediaType> type;
    HRESULT result = MFCreateMediaType(type.GetAddressOf());
    if (FAILED(result))
        return false;

    result = type->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    if (FAILED(result))
        return false;

    result = type->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
    if (FAILED(result))
        return false;

    result = m_reader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, type.Get());
    if (SUCCEEDED(result))
    {
        m_outputSubtype = MFVideoFormat_RGB32;
        return true;
    }

    return ConfigureOutputToNV12();
}

bool CMFVideoDecoderBackend::ConfigureOutputToNV12()
{
    if (!m_reader)
        return false;

    Microsoft::WRL::ComPtr<IMFMediaType> type;
    HRESULT result = MFCreateMediaType(type.GetAddressOf());
    if (FAILED(result))
        return false;

    result = type->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    if (FAILED(result))
        return false;

    result = type->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_NV12);
    if (FAILED(result))
        return false;

    result = m_reader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, type.Get());
    if (SUCCEEDED(result))
    {
        m_outputSubtype = MFVideoFormat_NV12;
        return true;
    }

    return false;
}
bool CMFVideoDecoderBackend::QuerySizeFromCurrentType()
{
    if (!m_reader)
        return false;

    Microsoft::WRL::ComPtr<IMFMediaType> currentType;
    HRESULT result = m_reader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, currentType.GetAddressOf());
    if (FAILED(result))
        return false;

    //  실제 subtype 갱신 (중요)
    GUID actualSubtype = GUID_NULL;
    if (SUCCEEDED(currentType->GetGUID(MF_MT_SUBTYPE, &actualSubtype)))
        m_outputSubtype = actualSubtype;

    UINT32 frameWidth = 0;
    UINT32 frameHeight = 0;
    result = MFGetAttributeSize(currentType.Get(), MF_MT_FRAME_SIZE, &frameWidth, &frameHeight);
    if (FAILED(result))
        return false;

    m_width = frameWidth;
    m_height = frameHeight;

    //  stride 갱신: RGB32 / NV12 둘 다 시도
    m_strideBytes = 0;

    UINT32 strideUnsigned = 0;
    if (SUCCEEDED(currentType->GetUINT32(MF_MT_DEFAULT_STRIDE, &strideUnsigned)))
    {
        const INT32 strideSigned = (INT32)strideUnsigned;
        m_strideBytes = (LONG)strideSigned;
    }

    if (m_strideBytes == 0)
    {
        if (m_outputSubtype == MFVideoFormat_RGB32)
            m_strideBytes = (LONG)m_width * 4;
        else if (m_outputSubtype == MFVideoFormat_NV12)
            m_strideBytes = (LONG)m_width; // fallback (정렬 스트라이드는 위에서 잡히는 경우가 많음)
        else
            m_strideBytes = 0; // 알 수 없는 포맷은 나중에 처리
    }

    return (m_width > 0 && m_height > 0);
}

bool CMFVideoDecoderBackend::SeekSeconds(float seconds)
{
    if (seconds < 0.0f)
        seconds = 0.0f;

    //  디코드 스레드와 동기화
    std::lock_guard<std::mutex> lock(m_readerMutex);

    if (!m_isOpened || !m_reader)
        return false;

    m_firstPtsMs = UINT64_MAX;

    const LONGLONG targetHns = (LONGLONG)(seconds * 10000000.0f);

    PROPVARIANT positionVar;
    PropVariantInit(&positionVar);
    positionVar.vt = VT_I8;
    positionVar.hVal.QuadPart = targetHns;

    const HRESULT hr = m_reader->SetCurrentPosition(GUID_NULL, positionVar);
    PropVariantClear(&positionVar);
    if (FAILED(hr))
        return false;

    m_reader->Flush(MF_SOURCE_READER_FIRST_VIDEO_STREAM);
    return true;
}

bool CMFVideoDecoderBackend::DecodeNextRGBA(
    std::vector<std::uint8_t>& outRgba,
    std::uint32_t& outWidth,
    std::uint32_t& outHeight,
    std::uint64_t& outPtsMs,
    bool& outEnded)
{
    std::lock_guard<std::mutex> decodeGateLock(m_decodeGate); //  핵심
    outEnded = false;
    outWidth = 0;
    outHeight = 0;
    outPtsMs = 0;

    if (IsStopRequested())
    {
        outEnded = true;
        return false;
    }

    Microsoft::WRL::ComPtr<IMFSourceReader> readerLocal;
    {
        std::lock_guard<std::mutex> lock(m_readerMutex);
        readerLocal = m_reader; // 로컬로 잡아두면 Close 중이라도 최소한 포인터 경쟁이 줄어듦
    }

    if (!m_isOpened || !readerLocal)
    {
        outEnded = true;
        return false;
    }

    const int maxReadAttempts = 8;

    for (int attemptIndex = 0; attemptIndex < maxReadAttempts; ++attemptIndex)
    {
        if (IsStopRequested())
        {
            outEnded = true;
            return false;
        }

        DWORD streamIndex = 0;
        DWORD flags = 0;
        LONGLONG timeStampHns = 0;

        Microsoft::WRL::ComPtr<IMFSample> sample;

        HRESULT readResult = readerLocal->ReadSample(
            MF_SOURCE_READER_FIRST_VIDEO_STREAM,
            0,
            &streamIndex,
            &flags,
            &timeStampHns,
            sample.GetAddressOf());

        if (IsStopRequested())
        {
            outEnded = true;
            return false;
        }

        if (FAILED(readResult))
            return false;

        if (flags & MF_SOURCE_READERF_ENDOFSTREAM)
        {
            if (m_isLoop)
            {
                SeekSeconds(0.0f);
                // 루프 직후엔 샘플이 바로 안 나올 수 있어서 계속 읽기 루프로 진행
                continue;
            }

            outEnded = true;
            return false;
        }

        if (flags & MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED)
        {
            // 스트림 타입이 바뀌면 크기/stride 갱신 필요
            QuerySizeFromCurrentType();
            // 다음 ReadSample로 계속
            continue;
        }

        if (!sample)
        {
            // STREAMTICK 등으로 sample이 null일 수 있음
            continue;
        }

        Microsoft::WRL::ComPtr<IMFMediaBuffer> buffer;
        HRESULT bufferResult = sample->ConvertToContiguousBuffer(buffer.GetAddressOf());
        if (FAILED(bufferResult))
            return false;

        ScopedBufferLock lock;
        if (!lock.Lock(buffer.Get()))
            return false;

        outWidth = m_width;
        outHeight = m_height;

        const std::uint64_t rawPtsMs = (std::uint64_t)(timeStampHns / 10000); // 100ns -> ms
        if (m_firstPtsMs == UINT64_MAX)
            m_firstPtsMs = rawPtsMs;

        outPtsMs = rawPtsMs - m_firstPtsMs;

        bool decodeSuccess = false;

        if (m_outputSubtype == MFVideoFormat_NV12)
        {
            const LONG strideBytesSigned = (m_strideBytes != 0) ? m_strideBytes : (LONG)m_width;
            const size_t absStrideBytes = (size_t)std::abs(strideBytesSigned);

            const size_t neededBytes =
                absStrideBytes * (size_t)m_height +
                absStrideBytes * (size_t)(m_height / 2);

            if ((size_t)lock.curLen >= neededBytes)
            {
                ConvertNV12ToRGBA_WithStride(
                    (const std::uint8_t*)lock.dataPtr,
                    m_width,
                    m_height,
                    (std::uint32_t)absStrideBytes,
                    outRgba);

                decodeSuccess = true;
            }
        }
        else // RGB32 (대개 BGRA 메모리)
        {
            const LONG strideBytes = (m_strideBytes != 0) ? m_strideBytes : (LONG)m_width * 4;
            const size_t absStride = (size_t)std::abs(strideBytes);
            const size_t minNeededBytes = absStride * (size_t)m_height;

            if ((size_t)lock.curLen >= minNeededBytes && strideBytes != 0)
            {
                outRgba.resize((size_t)m_width * (size_t)m_height * 4);

                const std::uint8_t* srcBase = (const std::uint8_t*)lock.dataPtr;

                // MF RGB32는 보통 BGRA 바이트 순서임
                auto CopyOneRowBGRAtoRGBA = [&](const std::uint8_t* srcRow, std::uint8_t* dstRow)
                    {
                        for (std::uint32_t colIndex = 0; colIndex < m_width; ++colIndex)
                        {
                            const size_t srcIndex = (size_t)colIndex * 4;
                            const size_t dstIndex = (size_t)colIndex * 4;

                            const std::uint8_t blueValue = srcRow[srcIndex + 0];
                            const std::uint8_t greenValue = srcRow[srcIndex + 1];
                            const std::uint8_t redValue = srcRow[srcIndex + 2];
                            // const std::uint8_t alphaValue = srcRow[srcIndex + 3];

                            dstRow[dstIndex + 0] = redValue;
                            dstRow[dstIndex + 1] = greenValue;
                            dstRow[dstIndex + 2] = blueValue;
                            dstRow[dstIndex + 3] = 255; // ★ 무조건 불투명
                        }
                    };
                if (strideBytes > 0)
                {
                    for (std::uint32_t rowIndex = 0; rowIndex < m_height; ++rowIndex)
                    {
                        const std::uint8_t* srcRow = srcBase + (size_t)rowIndex * absStride;
                        std::uint8_t* dstRow = outRgba.data() + (size_t)rowIndex * (size_t)m_width * 4;
                        CopyOneRowBGRAtoRGBA(srcRow, dstRow);
                    }
                }
                else
                {
                    // stride가 음수면 top-down. 첫 줄이 버퍼 끝쪽에 있음
                    const std::uint8_t* srcTopRow = srcBase + absStride * (size_t)(m_height - 1);

                    for (std::uint32_t rowIndex = 0; rowIndex < m_height; ++rowIndex)
                    {
                        const std::uint8_t* srcRow = srcTopRow - (size_t)rowIndex * absStride;
                        std::uint8_t* dstRow = outRgba.data() + (size_t)rowIndex * (size_t)m_width * 4;
                        CopyOneRowBGRAtoRGBA(srcRow, dstRow);
                    }
                }

                decodeSuccess = true;
            }
        }

        return decodeSuccess;
    }

    // attempts 초과: 사실상 프레임이 안 나오는 상태
    outEnded = true;
    return false;
}
