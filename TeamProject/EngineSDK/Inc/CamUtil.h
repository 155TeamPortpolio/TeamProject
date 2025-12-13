#pragma once

#include "Camera_Enum.h"
#include "Camera_Struct.h"

NS_BEGIN(Engine)

class ENGINE_DLL CamUtil
{
public:
	// "현재 시간(time)이 키프레임들 중 어느 구간(두 키 사이)에 속하는지"를 찾아서, 그 구간 안에서의 "보간 비율(0~1)"까지 같이 계산해주는 함수.
	// segmentIdx: 시작 키 인덱스 (구간은 [segmentIdx, segmentIdx + 1])
	// normalizedTime: 그 구간 안에서의 진행률 u (0~1)
	static CamKeySegment FindKeySegment(const vector<CamKeyFrame>& keyframes, float time);
};

NS_END