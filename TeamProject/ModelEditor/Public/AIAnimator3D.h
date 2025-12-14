#pragma once
#include "ModelEditor_Defines.h"
#include "Assimps.h"
#include "Base.h"

class CAIClipData final :
	public CBase
{
private:
	CAIClipData();
	virtual ~CAIClipData() DEFAULT;
	
public:
	HRESULT Initialize();

private:


public:
	static CAIClipData* Create(const aiAnimation* pAIAnimation);
	virtual void Free() override;
};

