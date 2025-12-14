#pragma once
#include "AnimationTool_Defines.h"
#include "AnimationClip.h"
#include "Helper_Func.h"

NS_BEGIN(Engine)

class CAnimationClipEX :
	public CAnimationClip
{
protected:
	CAnimationClipEX();
	CAnimationClipEX(const string& Subject);
	virtual ~CAnimationClipEX() DEFAULT;

public:
	HRESULT Initialize();
	
public:
	virtual void Render_GUI();

public:
	static CAnimationClipEX* Create();
	virtual void Free() override;
};
NS_END
