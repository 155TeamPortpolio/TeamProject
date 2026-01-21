#include "pch.h"
#include "CamLoader.h"
#include "CamDirector.h"
#include "GameInstance.h"

namespace fs = filesystem;

namespace
{
    static const CamShakePreset kShakePresets[] =
    {
        CamShakePreset{0.10f, 0.00030f, 0.050f, 0.06f, 0.00018f, 16.f, 0.080f, 0.140f}, // TapSoft
        CamShakePreset{0.16f, 0.00042f, 0.055f, 0.08f, 0.00022f, 16.f, 0.090f, 0.160f}, // HitLight
        CamShakePreset{0.22f, 0.00060f, 0.060f, 0.10f, 0.00030f, 15.f, 0.100f, 0.180f}, // HitNormal
        CamShakePreset{0.40f, 0.00095f, 0.070f, 0.18f, 0.00048f, 14.f, 0.120f, 0.220f}, // HitHeavy
        CamShakePreset{0.55f, 0.00115f, 0.075f, 0.22f, 0.00060f, 13.f, 0.135f, 0.260f}, // HitCrit

        CamShakePreset{0.32f, 0.00105f, 0.080f, 0.16f, 0.00055f, 12.f, 0.140f, 0.260f}, // ExplosionSmall
        CamShakePreset{0.75f, 0.00185f, 0.085f, 0.30f, 0.00095f, 11.f, 0.160f, 0.320f}, // ExplosionBig

        CamShakePreset{0.20f, 0.00095f, 0.070f, 0.08f, 0.00035f, 12.f, 0.110f, 0.220f}, // LandingLight
        CamShakePreset{0.48f, 0.00155f, 0.080f, 0.14f, 0.00055f, 11.f, 0.140f, 0.300f}, // LandingHeavy

        CamShakePreset{0.12f, 0.00040f, 0.060f, 0.10f, 0.00025f, 18.f, 0.080f, 0.140f}, // Dash

        // ---- Roar (long) ----
        CamShakePreset{0.06f, 0.00020f, 0.060f, 0.14f, 0.00040f, 9.f, 2.000f, 0.600f}, // Roar2s
        CamShakePreset{0.07f, 0.00022f, 0.060f, 0.16f, 0.00045f, 9.f, 2.500f, 0.700f}, // Roar25s
    };

    static const CamZoomPreset kZoomPresets[] =
    {
        CamZoomPreset{0.45f, 0.050f, 0.120f}, // TapSoft
        CamZoomPreset{0.70f, 0.045f, 0.140f}, // HitLight
        CamZoomPreset{0.90f, 0.045f, 0.150f}, // HitNormal
        CamZoomPreset{1.30f, 0.050f, 0.180f}, // HitHeavy
        CamZoomPreset{1.70f, 0.055f, 0.200f}, // HitCrit

        CamZoomPreset{1.10f, 0.055f, 0.220f}, // ExplosionSmall
        CamZoomPreset{2.10f, 0.060f, 0.260f}, // ExplosionBig

        CamZoomPreset{0.80f, 0.050f, 0.170f}, // LandingLight
        CamZoomPreset{1.40f, 0.055f, 0.230f}, // LandingHeavy

        CamZoomPreset{0.55f, 0.045f, 0.110f}, // Dash

        // ---- Roar (long) ----
        CamZoomPreset{0.18f, 0.080f, 2.000f},  // Roar2s (»ìÂ¦¸¸)
        CamZoomPreset{0.22f, 0.080f, 2.500f},  // Roar25s
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
        req.blendOutSec = 0.25f;
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