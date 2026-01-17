#include "pch.h"
#include "CamLoader.h"
#include "CamDirector.h"

namespace fs = filesystem;

namespace
{
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
            req.blendInSec  = 0.05f;
            req.blendOutSec = 0.45f;
            req.resetTime   = true;
            req.returnMode  = CamReturnMode::SnapToEnd;
            return req;
        }

        if (StartsWith(key, "Ultimate/"))
        {
            req.blendInSec  = 0.f;
            req.blendOutSec = 0.5f;
            req.resetTime   = true;
            req.returnMode  = CamReturnMode::RestorePrev;
            return req;
        }

        req.blendInSec  = 0.25f;
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

            CamDirector().Register(key, path, req);
        }
    }
}

void CamLoader::Load()
{
    LoadFolder("../../Resources/Data/Camera");
}