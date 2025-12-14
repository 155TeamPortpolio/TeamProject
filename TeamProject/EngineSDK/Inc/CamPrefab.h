#pragma once

#include "CamUtil.h"

namespace CamPrefab
{
	bool Save(const filesystem::path& path, const CamSequenceDesc& seq, string& outErrorMsg);
	bool Load(const filesystem::path& path, CamSequenceDesc& outSeq, string& outErrorMsg);
}

// Save »©°í + Camera °ü·Ã 