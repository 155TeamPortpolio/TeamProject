#pragma once

#include "CamObject.h"

NS_BEGIN(Engine)
class CCamera;
NS_END

namespace DebugCamTrace
{
	void Trace_EveryFrame(CCamera* a, const char* aTag, CCamera* b, const char* bTag);
}