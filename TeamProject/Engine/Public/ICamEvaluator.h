#pragma once

#include "Base.h"
#include "CamUtil.h"

NS_BEGIN(Engine)

class ENGINE_DLL ICamPosEvaluator abstract : public CBase
{
protected:
    ICamPosEvaluator() DEFAULT;
    virtual ~ICamPosEvaluator() DEFAULT;

public:
    virtual bool     Build(const vector<CamKeyFrame>& keys) PURE;
	virtual _vector3 Evaluate(float t) const PURE;
};

class ENGINE_DLL ICamRotEvaluator abstract : public CBase
{
protected:
    ICamRotEvaluator() DEFAULT;
    virtual ~ICamRotEvaluator() DEFAULT;

public:
    virtual bool       Build(const vector<CamKeyFrame>& keys) PURE;
    virtual Quaternion Evaluate(float t) const PURE;
};

class ENGINE_DLL ICamFovEvaluator abstract : public CBase
{
protected:
    ICamFovEvaluator() DEFAULT;
    virtual ~ICamFovEvaluator() DEFAULT;

public:
    virtual bool  Build(const vector<CamKeyFrame>& keys) PURE;
    virtual float Evaluate(float t) const PURE;
};

NS_END