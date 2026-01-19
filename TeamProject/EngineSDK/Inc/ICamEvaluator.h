#pragma once

#include "Base.h"
#include "CamUtil.h"

NS_BEGIN(Engine)

class ENGINE_DLL ICamPosEvaluator abstract : public CBase
{
protected:
	ICamPosEvaluator() = default;
	virtual ~ICamPosEvaluator() = default;

public:
	virtual bool     Build(const vector<CamKeyFrame>& keys) = 0;
	virtual _vector3 Evaluate(float t) const = 0;
};

class ENGINE_DLL ICamRotEvaluator abstract : public CBase
{
protected:
    ICamRotEvaluator() = default;
    virtual ~ICamRotEvaluator() = default;

public:
    virtual bool       Build(const vector<CamKeyFrame>& keys) = 0;
    virtual Quaternion Evaluate(float t) const = 0;
};

class ENGINE_DLL ICamFovEvaluator abstract : public CBase
{
protected:
    ICamFovEvaluator() = default;
    virtual ~ICamFovEvaluator() = default;

public:
    virtual bool  Build(const vector<CamKeyFrame>& keys) = 0;
    virtual float Evaluate(float t) const = 0;
};

NS_END