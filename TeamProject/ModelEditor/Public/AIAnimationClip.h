#pragma once
#include "ModelEditor_Defines.h"
#include "AnimationClip.h"
#include "Assimps.h"

NS_BEGIN(ModelEdit)
class CAIAnimationClip :
	public CAnimationClip
{
private:
	CAIAnimationClip();
	virtual ~CAIAnimationClip() DEFAULT;

public:
	virtual HRESULT Initialize(const aiAnimation* pAIAnimation, class CAIModelData* pAIModelData);
	void Save_File(ofstream& ofs);

private:

public:
	static CAnimationClip* Create(const aiAnimation* pAIAnimation, class CAIModelData* pAIModelData);
	virtual void Free() override;
};

NS_END