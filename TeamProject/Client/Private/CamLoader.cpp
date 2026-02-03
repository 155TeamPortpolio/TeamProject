#include "pch.h"
#include "CamLoader.h"
#include "CamDirector.h"
#include "GameInstance.h"

namespace fs = filesystem;

namespace
{   // kickRotDeg, kickPos, kickDur, noiseRotDeg, noisePos, noiseFreq, dur, fadeOutSec                             
    static const CamShakePreset kShakePresets[] =
    {   
        {0.10f, 0.00030f, 0.050f, 0.06f, 0.00018f, 16.f, 0.080f, 0.140f}, // TapSoft
        {0.16f, 0.00042f, 0.055f, 0.08f, 0.00022f, 16.f, 0.090f, 0.160f}, // HitLight
        {0.22f, 0.00060f, 0.060f, 0.10f, 0.00030f, 15.f, 0.100f, 0.180f}, // HitNormal
        {0.40f, 0.00095f, 0.070f, 0.18f, 0.00048f, 14.f, 0.120f, 0.220f}, // HitHeavy
        {0.60f, 0.00135f, 0.080f, 0.24f, 0.00070f, 13.f, 0.150f, 0.260f}, // HitCrit

        {0.32f, 0.00105f, 0.080f, 0.16f, 0.00055f, 12.f, 0.140f, 0.260f}, // ExplosionSmall
        {0.75f, 0.00185f, 0.085f, 0.30f, 0.00095f, 11.f, 0.160f, 0.320f}, // ExplosionBig
        {1.10f, 0.00260f, 0.095f, 0.42f, 0.00125f, 10.5f, 0.450f, 0.360f}, // ExplosionHuge
        {1.65f, 0.00380f, 0.110f, 0.55f, 0.00170f, 9.5f, 0.700f, 0.550f},  // ExplosionMega

        {0.20f, 0.00095f, 0.070f, 0.08f, 0.00035f, 12.f, 0.110f, 0.220f}, // LandingLight
        {0.48f, 0.00155f, 0.080f, 0.14f, 0.00055f, 11.f, 0.140f, 0.300f}, // LandingHeavy
        {0.65f, 0.00195f, 0.090f, 0.20f, 0.00075f, 10.5f, 0.220f, 0.320f}, // LandingCrush

        {0.12f, 0.00040f, 0.060f, 0.10f, 0.00025f, 18.f, 0.080f, 0.140f}, // Dash
        {0.10f, 0.00035f, 0.050f, 0.08f, 0.00022f, 20.f, 0.090f, 0.140f}, // Dodge
        {0.04f, 0.00012f, 0.040f, 0.03f, 0.00010f, 24.f, 0.060f, 0.080f}, // SprintStep

        {0.28f, 0.00085f, 0.055f, 0.11f, 0.00030f, 18.f, 0.120f, 0.160f}, // Parry
        {0.55f, 0.00145f, 0.070f, 0.18f, 0.00055f, 15.f, 0.180f, 0.220f}, // GuardBreak
        {0.35f, 0.00110f, 0.085f, 0.12f, 0.00040f, 12.f, 0.160f, 0.180f}, // Knockback
        {0.12f, 0.00035f, 0.060f, 0.16f, 0.00055f, 8.f, 0.800f, 0.400f},  // Stun

        {0.08f, 0.00025f, 0.060f, 0.12f, 0.00045f, 7.f, 0.900f, 0.400f},  // UltimateStart
        {2.20f, 0.00500f, 0.120f, 0.75f, 0.00230f, 9.f, 0.800f, 0.550f},  // UltimateImpact
        {0.10f, 0.00030f, 0.060f, 0.18f, 0.00080f, 8.f, 1.200f, 0.800f},  // UltimateEnd

        {0.05f, 0.00020f, 0.070f, 0.20f, 0.00090f, 6.f, 0.800f, 0.600f},  // EarthquakeShort
        {0.04f, 0.00018f, 0.070f, 0.18f, 0.00085f, 5.f, 2.500f, 1.200f},  // EarthquakeLong

        {0.05f, 0.00018f, 0.060f, 0.12f, 0.00035f, 10.f, 1.000f, 0.500f}, // Roar1S
        {0.06f, 0.00019f, 0.060f, 0.13f, 0.00038f, 9.5f, 1.500f, 0.550f}, // Roar15S
        {0.06f, 0.00020f, 0.060f, 0.14f, 0.00040f, 9.f, 2.000f, 0.600f},  // Roar2S
        {0.07f, 0.00022f, 0.060f, 0.16f, 0.00045f, 9.f, 2.500f, 0.700f},  // Roar25S
        {0.08f, 0.00024f, 0.060f, 0.20f, 0.00055f, 8.5f, 4.000f, 1.000f}, // Roar4S
    };

    static const CamZoomPreset kZoomPresets[] =
    {   // amountDeg, attackSec, releaseSec
        {0.45f, 0.050f, 0.120f}, // TapSoft
        {0.70f, 0.045f, 0.140f}, // HitLight
        {0.90f, 0.045f, 0.150f}, // HitNormal
        {1.30f, 0.050f, 0.180f}, // HitHeavy
        {1.70f, 0.055f, 0.200f}, // HitCrit

        {1.10f, 0.055f, 0.220f}, // ExplosionSmall
        {2.10f, 0.060f, 0.260f}, // ExplosionBig
        {3.20f, 0.065f, 0.320f}, // ExplosionHuge
        {4.60f, 0.070f, 0.420f}, // ExplosionMega

        {0.80f, 0.050f, 0.170f}, // LandingLight
        {1.40f, 0.055f, 0.230f}, // LandingHeavy
        {1.90f, 0.060f, 0.280f}, // LandingCrush

        {0.55f, 0.045f, 0.110f}, // Dash
        {0.50f, 0.045f, 0.110f}, // Dodge
        {0.18f, 0.030f, 0.080f}, // SprintStep

        {0.95f, 0.040f, 0.140f}, // Parry
        {1.55f, 0.050f, 0.180f}, // GuardBreak
        {1.15f, 0.050f, 0.160f}, // Knockback
        {0.60f, 0.060f, 0.220f}, // Stun

        {0.35f, 0.080f, 0.900f}, // UltimateStart
        {6.00f, 0.050f, 0.350f}, // UltimateImpact
        {0.40f, 0.080f, 1.200f}, // UltimateEnd

        {-0.25f, 0.080f, 0.800f}, // EarthquakeShort
        {-0.35f, 0.100f, 2.200f}, // EarthquakeLong

        {0.16f, 0.080f, 1.000f}, // Roar1S
        {0.19f, 0.080f, 1.500f}, // Roar15S
        {0.18f, 0.080f, 2.000f}, // Roar2S
        {0.22f, 0.080f, 2.500f}, // Roar25S
        {0.26f, 0.090f, 4.000f}, // Roar4S
    };

    _bool StartsWith(const string& s, const char* prefix)
    {
        const size_t n = strlen(prefix);
        if (s.size() < n) return false;
        return memcmp(s.data(), prefix, n) == 0;
    }

    CamSequenceRequestDesc ResolveDefaultReq(const string& key)
    {
        CamSequenceRequestDesc req{};
        req.returnCamType = CamType::None;

        if (StartsWith(key, "CutScene/"))
        {
            req.blendInSec  = 0.f;
            req.blendOutSec = 0.35f;
            req.resetTime   = true;
            req.returnMode  = CamReturnMode::RestorePrev;
            return req;
        }

        if (StartsWith(key, "Intro/"))
        {
            req.blendInSec  = 0.0f;
            req.blendOutSec = 0.5f;
            req.resetTime   = true;
            req.returnMode  = CamReturnMode::SnapToEnd;
            return req;
        }

        if (StartsWith(key, "Ultimate/"))
        {
            req.blendInSec  = 0.f;
            req.blendOutSec = 0.f;
            req.resetTime   = true;
            req.returnMode  = CamReturnMode::RestorePrev;
            return req;
        }

        req.blendInSec  = 0.f;
        req.blendOutSec = 0.f;
        req.resetTime   = true;
        req.returnMode  = CamReturnMode::SnapToEnd;
        return req;
    }

    void LoadFolder(const fs::path& folder)
    {
        if (!fs::exists(folder) || !fs::is_directory(folder)) return;

        for (const auto& it : fs::recursive_directory_iterator(folder))
        {
            if (!it.is_regular_file()) continue;

            const auto& path = it.path();
            if (path.extension() != ".cam") continue;

            fs::path rel = fs::relative(path, folder);
            rel.replace_extension();

            const string key = rel.generic_string();
            const CamSequenceRequestDesc req = ResolveDefaultReq(key);

            CamDirector()->Register(key, path, req);
        }
    }
}

void CamLoader::Load()
{
    LoadFolder("../../Resources/Data/Camera");
    CameraManager()->RegisterShakePresets(kShakePresets, ENUM(CamShakeType::End));
    CameraManager()->RegisterZoomPresets(kZoomPresets,   ENUM(CamZoomType::End));
}