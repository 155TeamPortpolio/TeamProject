#pragma once
#include "AnimationTool_Defines.h"
#include "AnimationClip.h"
#include "Helper_Func.h"

NS_BEGIN(Engine)

class CAnimationClipEX :
	public CAnimationClip
{
protected:
	CAnimationClipEX(const string& Subject);
	CAnimationClipEX();
	virtual ~CAnimationClipEX() DEFAULT;

public:
	//virtual HRESULT Initialize();
	
public:
	virtual void Render_GUI();

public:
	static CAnimationClipEX* Create();
	virtual void Free() override;
};
NS_END
