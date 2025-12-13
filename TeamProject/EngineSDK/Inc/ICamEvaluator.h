#pragma once

#include "CamUtil.h"

NS_BEGIN(Engine)

class ENGINE_DLL ICamPosEvaluator
{
public:
	virtual ~ICamPosEvaluator() = default;
	virtual bool Build(const vector<CamKeyFrame>& keys) = 0;
	virtual _vector3 Evaluate(float t) const = 0;
};

class ENGINE_DLL ICamRotEvaluator
{
public:
	virtual ~ICamRotEvaluator() = default;
	virtual bool Build(const vector<CamKeyFrame>& keys) = 0;
	virtual Quaternion Evaluate(float t) const = 0;
};

class ENGINE_DLL ICamFovEvaluator
{
public:
	virtual ~ICamFovEvaluator() = default;
	virtual bool Build(const vector<CamKeyFrame>& keys) = 0;
	virtual float Evaluate(float t) const = 0;
};

NS_END