#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CGachaProps :
    public CGameObject
{
private:
    CGachaProps();
    CGachaProps(const CGachaProps& rhs);
    virtual ~CGachaProps() DEFAULT;

public:
    void    SetupInitialSequence();
    void    PlayTVSequence();
    void    PlayStageSpin(_int index);
    void    ResetTarget();

public:
    virtual HRESULT Initialize_Prototype(vector<GACHA_RESULT_DESC>* Desc);
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake()                     override;
    virtual void    Priority_Update(_float dt)  override;
    virtual void    Update(_float dt)           override;
    virtual void    Late_Update(_float dt)      override;

private:
    HRESULT Initialize_GachaPrototype();
    void Add_GachaProps();

private:
    vector<GACHA_RESULT_DESC>*    m_pResultDesc = nullptr;

private:
    class CGachaTV*                 m_pTV = nullptr;
    class CGachaStage*              m_pStage = nullptr;

public:
    static CGachaProps* Create(vector<GACHA_RESULT_DESC>* Desc);
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END