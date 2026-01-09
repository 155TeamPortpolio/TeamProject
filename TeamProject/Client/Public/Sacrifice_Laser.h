#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CSacrifice_Laser :
    public CGameObject
{
private:
    CSacrifice_Laser();
    CSacrifice_Laser(const CSacrifice_Laser& rhg);
    virtual ~CSacrifice_Laser() DEFAULT;

public:
    HRESULT Initialize_Prototype()override;
    HRESULT Initialize(INIT_DESC* pArg)override;
    void    Awake() override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;
    void    Render_GUI() override;

public:
    static CSacrifice_Laser* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    void Free() override;

private:
    /* 레이저 시작 지점, 레이저 본체, 레이저 끝*/
};

NS_END