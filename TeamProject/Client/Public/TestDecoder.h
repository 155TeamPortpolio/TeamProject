#pragma once
#include "IVideoDecoderBackend.h"
#include "Decoder.h"
NS_BEGIN(Client)
class CTestDecoder :
    public IVideoDecoderBackend
{
public:
    CTestDecoder() = default;
    virtual ~CTestDecoder() = default;

public:
    bool Open(const std::string& filePath) override
    {
        (void)filePath;

        m_isOpened = true;
        m_isLoop = false;

        m_width = 1280;
        m_height = 720;

        m_fpsNumerator = 30;
        m_fpsDenominator = 1;

        m_frameIndex = 0;
        m_durationMs = 25000; // 5초짜리 더미 영상
        m_startClock = std::chrono::steady_clock::now();

        return true;
    }

    void Close() override
    {
        m_isOpened = false;
        m_frameIndex = 0;
    }

    void SetLoop(bool loop) override
    {
        m_isLoop = loop;
    }

    void SeekSeconds(float seconds) override
    {
        if (!m_isOpened)
            return;

        if (seconds < 0.f)
            seconds = 0.f;

        const std::uint64_t targetMs = (std::uint64_t)(seconds * 1000.0f);
        const std::uint64_t frameDurationMs = GetFrameDurationMs();

        m_frameIndex = (std::uint64_t)(targetMs / frameDurationMs);
        // "시간 기준"으로도 맞추고 싶으면 startClock 보정도 가능하지만
        // 더미는 간단히 프레임 인덱스만 이동시키자.
    }

    bool DecodeNextRGBA(
        std::vector<std::uint8_t>& outRgba,
        std::uint32_t& outWidth,
        std::uint32_t& outHeight,
        std::uint64_t& outPts,
        bool& outEnded) override
    {
        outEnded = false;

        if (!m_isOpened)
        {
            outEnded = true;
            return false;
        }

        const std::uint64_t frameDurationMs = GetFrameDurationMs();
        const std::uint64_t ptsMs = m_frameIndex * frameDurationMs;

        if (ptsMs >= m_durationMs)
        {
            if (m_isLoop)
            {
                m_frameIndex = 0;
                outEnded = false;
            }
            else
            {
                outEnded = true;
                return false;
            }
        }

        // 출력 메타
        outWidth = m_width;
        outHeight = m_height;
        outPts = m_frameIndex * frameDurationMs;

        // RGBA 버퍼 준비
        const std::size_t pixelCount = (std::size_t)m_width * (std::size_t)m_height;
        outRgba.resize(pixelCount * 4);

        // 시간에 따른 색 변화
        const float timeSec = (float)outPts / 1000.0f;
        const float wave = 0.5f + 0.5f * std::sin(timeSec * 2.0f);

        const std::uint8_t baseRed = (std::uint8_t)(40 + 160 * wave);
        const std::uint8_t baseGreen = (std::uint8_t)(30 + 120 * (1.0f - wave));
        const std::uint8_t baseBlue = (std::uint8_t)(60 + 140 * wave);

        const std::uint32_t tileSize = 64;

        for (std::uint32_t y = 0; y < m_height; ++y)
        {
            for (std::uint32_t x = 0; x < m_width; ++x)
            {
                const bool isOddTile =
                    (((x / tileSize) + (y / tileSize)) % 2) == 1;

                const std::uint8_t tileBias = isOddTile ? 40 : 0;

                const std::size_t index = ((std::size_t)y * (std::size_t)m_width + (std::size_t)x) * 4;
                outRgba[index + 0] = (std::uint8_t)std::min<int>(255, baseRed + tileBias);
                outRgba[index + 1] = (std::uint8_t)std::min<int>(255, baseGreen + tileBias);
                outRgba[index + 2] = (std::uint8_t)std::min<int>(255, baseBlue + tileBias);
                outRgba[index + 3] = 255;
            }
        }

        // 다음 프레임로
        ++m_frameIndex;
        return true;
    }

private:
    std::uint64_t GetFrameDurationMs() const
    {
        // fps = fpsNumerator / fpsDenominator
        // frameDuration = 1000ms / fps
        const double fps = (double)m_fpsNumerator / (double)m_fpsDenominator;
        const double frameMs = 1000.0 / fps;
        return (std::uint64_t)(frameMs + 0.5);
    }

private:
    bool m_isOpened = false;
    bool m_isLoop = false;

    std::uint32_t m_width = 0;
    std::uint32_t m_height = 0;

    std::uint32_t m_fpsNumerator = 30;
    std::uint32_t m_fpsDenominator = 1;

    std::uint64_t m_frameIndex = 0;
    std::uint64_t m_durationMs = 0;

    std::chrono::steady_clock::time_point m_startClock{};

   public:
       static CTestDecoder* Create();
};

NS_END