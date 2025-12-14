#pragma once
#include "ModelEditor_Defines.h"
#include "Animator3D.h"
#include "Assimps.h"

NS_BEGIN(ModelEdit)

class CAIAnimator3D final :
	public CAnimator3D
{
private:
	CAIAnimator3D();
	virtual ~CAIAnimator3D() DEFAULT;
	
public:
	HRESULT Initialize(const aiScene* pAIScene, class CAIModelData* pAIModelData);

public:
	static CAIAnimator3D* Create(const aiScene* pAIScene, class CAIModelData* pAIModelData);
	virtual void Free() override;
};

NS_END