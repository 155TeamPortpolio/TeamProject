#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CUI_MeshBillboard : public CGameObject
{
private:
    CUI_MeshBillboard();
    CUI_MeshBillboard(const CUI_MeshBillboard& rhs);
    virtual ~CUI_MeshBillboard() DEFAULT;

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