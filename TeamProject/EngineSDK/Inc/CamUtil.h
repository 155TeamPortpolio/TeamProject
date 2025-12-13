#pragma once

#include "Camera_Enum.h"
#include "Camera_Struct.h"

NS_BEGIN(Engine)

class ENGINE_DLL CamUtil
{
public:
	static CamKeySegment FindKeySegment(const vector<CamKeyFrame>& keyframes, float time);
};

NS_END