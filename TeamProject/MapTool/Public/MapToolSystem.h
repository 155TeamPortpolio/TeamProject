#pragma once
#include "Base.h"

NS_BEGIN(MapTool)

class CMapToolSystem final : public CBase
{
private:
	CMapToolSystem();
	virtual ~CMapToolSystem() = default;

public:
    //HRESULT Initialize_Prototype() override;
    //HRESULT Initialize(INIT_DESC* pArg) override;
    //void Priority_Update(_float dt) override;
    //void Update(_float dt) override;
    //void Late_Update(_float dt) override;
    //virtual void Render_GUI() override;


public:
	static CMapToolSystem* Create();
	//virtual void Free() override;
};

NS_END