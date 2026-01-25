#pragma once
#include "DialogueNpc.h"

NS_BEGIN(Client)

class CElectricBoo :
    public CDialogueNpc
{
private:
    CElectricBoo();
    CElectricBoo(const CElectricBoo& rhs);
    virtual ~CElectricBoo() DEFAULT;

public:
    virtual void    Execute() override;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;

protected:
    virtual void    Add_DialoguePartner() override;

public:
    static CElectricBoo* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END