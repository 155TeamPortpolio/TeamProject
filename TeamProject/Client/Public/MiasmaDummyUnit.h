#pragma once
#include "Enemy.h"
#include "Defiler_Control.h"

NS_BEGIN(Client)
class CMiasmaDummyUnit :
    public CEnemy
{
private:
    CMiasmaDummyUnit();
    CMiasmaDummyUnit(const CMiasmaDummyUnit& rhg);
    virtual ~CMiasmaDummyUnit() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Awake() override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;
    void    Render_GUI()override;

public:
    DefilerDissolve& Get_Dissolve() { return m_Dissolve; }
    void Parried() override;
    void Update_Dissolve(_float dt);

private:
    DefilerDissolve m_Dissolve;

public:
    static CMiasmaDummyUnit* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};
NS_END
