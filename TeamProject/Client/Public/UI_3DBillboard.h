#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CUI_3DBillboard : public CGameObject
{
private:
    CUI_3DBillboard();
    CUI_3DBillboard(const CUI_3DBillboard& rhs);
    virtual ~CUI_3DBillboard() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;

public:
    static CGameObject* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END