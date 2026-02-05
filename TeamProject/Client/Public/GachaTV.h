#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CGachaTV :
    public CGameObject
{
private:
    CGachaTV();
    CGachaTV(const CGachaTV& rhs);
    virtual ~CGachaTV() DEFAULT;

public:
    void    PlayTVSequence();
    void    SetupInitialTVSequence();

public:
    virtual HRESULT Initialize_Prototype(vector<GACHA_RESULT_DESC>* Desc);
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake()                     override;
    virtual void    Priority_Update(_float dt)  override;
    virtual void    Update(_float dt)           override;
    virtual void    Late_Update(_float dt)      override;

private:
    void Add_TVScreen();

private:
    vector<GACHA_RESULT_DESC>* m_pResultDesc = nullptr;

private:
    class CGachaScreen* m_pScreen = nullptr;

public:
    static CGachaTV* Create(vector<GACHA_RESULT_DESC>* Desc);
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END