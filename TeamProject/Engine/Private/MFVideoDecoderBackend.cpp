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
    Close();

    std::wstring widePath = ConvertUtf8ToWide(filePath);
    if (widePath.empty())
        return false;

    Microsoft::WRL::ComPtr<IMFAttributes> attributes;
    HRESULT result = MFCreateAttributes(attributes.GetAddressOf(), 8);
    if (FAILED(result))
        return false;

    // 컨버터/비디오 프로세싱 활성화 (RGB32 성공률 ↑)
    attributes->SetUINT32(MF_READWRITE_DISABLE_CONVERTERS, FALSE);
    attributes->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, TRUE);
    attributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE);

    result = MFCreateSourceReaderFromURL(widePath.c_str(), attributes.Get(), m_reader.GetAddressOf());
    if (FAILED(result))
        return false;

    // 스트림 선택
    result = m_reader->SetStreamSelection(MF_SOURCE_READER_FIRST_VIDEO_STREAM, TRUE);
    if (FAILED(result))
        return false;

    // 출력 포맷 설정 (RGB32 시도 -> 실패 시 NV12)
    if (!ConfigureOutputToRGB32())
        return false;

    if (!QuerySizeFromCurrentType())
        return false;

    // duration (선택)
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

void CMFVideoDecoderBackend::Close()
{
    m_reader.Reset();

    m_isOpened = false;
    m_isLoop = false;

    m_firstPtsMs = UINT64_MAX;
    m_width = 0;
    m_height = 0;
    m_strideBytes = 0;
    m_outputSubtype = GUID_NULL;
    m_durationHns = -1;
}

void CMFVideoDecoderBackend::Free()
{
    Close();
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

    UINT32 frameWidth = 0;
    UINT32 frameHeight = 0;
    result = MFGetAttributeSize(currentType.Get(), MF_MT_FRAME_SIZE, &frameWidth, &frameHeight);
    if (FAILED(result))
        return false;

    m_width = frameWidth;
    m_height = frameHeight;

    m_strideBytes = 0;

    if (m_outputSubtype == MFVideoFormat_RGB32)
    {
        // MF_MT_DEFAULT_STRIDE는 UINT32로 저장되지만 실제 의미는 signed stride(음수 가능)
        UINT32 strideUnsigned = 0;
        HRESULT strideResult = currentType->GetUINT32(MF_MT_DEFAULT_STRIDE, &strideUnsigned);

        if (SUCCEEDED(strideResult))
        {
            // signed로 재해석
            const INT32 strideSigned = (INT32)strideUnsigned;
            m_strideBytes = (LONG)strideSigned;
        }

        if (m_strideBytes == 0)
        {
            // fallback: 보통은 -width*4 또는 width*4 중 하나
            m_strideBytes = (LONG)m_width * 4;
        }
    }

    return (m_width > 0 && m_height > 0);
}

bool CMFVideoDecoderBackend::SeekSeconds(float seconds)
{
    if (!m_isOpened || !m_reader)
        return false;

    if (seconds < 0.0f)
        seconds = 0.0f;

    m_firstPtsMs = UINT64_MAX;

    const LONGLONG targetHns = (LONGLONG)(seconds * 10000000.0f);

    PROPVARIANT positionVar;
    PropVariantInit(&positionVar);
    positionVar.vt = VT_I8;
    positionVar.hVal.QuadPart = targetHns;

    const HRESULT hr = m_reader->SetCurrentPosition(GUID_NULL, positionVar);
    PropVariantClear(&positionVar);

    return SUCCEEDED(hr);
}

bool CMFVideoDecoderBackend::DecodeNextRGBA(
    std::vector<std::uint8_t>& outRgba,
    std::uint32_t& outWidth,
    std::uint32_t& outHeight,
    std::uint64_t& outPtsMs,
    bool& outEnded)
{
    outEnded = false;
    outWidth = 0;
    outHeight = 0;
    outPtsMs = 0;

    if (!m_isOpened || !m_reader)
    {
        outEnded = true;
        return false;
    }

    // 일부 파일은 ReadSample에서 STREAMTICK / MEDIATYPECHANGED 등을 내고 sample이 없을 수 있음
    // 그래서 몇 번은 “계속 읽기”를 허용
    const int maxReadAttempts = 8;

    for (int attemptIndex = 0; attemptIndex < maxReadAttempts; ++attemptIndex)
    {
        DWORD streamIndex = 0;
        DWORD flags = 0;
        LONGLONG timeStampHns = 0;

        Microsoft::WRL::ComPtr<IMFSample> sample;
        HRESULT readResult = m_reader->ReadSample(
            MF_SOURCE_READER_FIRST_VIDEO_STREAM,
            0,
            &streamIndex,
            &flags,
            &timeStampHns,
            sample.GetAddressOf());

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
            const size_t neededBytes = (size_t)m_width * (size_t)m_height * 3 / 2;
            if ((size_t)lock.curLen >= neededBytes)
            {
                ConvertNV12ToRGBA((const std::uint8_t*)lock.dataPtr, m_width, m_height, outRgba);
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
