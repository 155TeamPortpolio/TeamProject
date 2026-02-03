#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CGachaStage :
    public CGameObject
{
private:
    CGachaStage();
    CGachaStage(const CGachaStage& rhs);
    virtual ~CGachaStage() DEFAULT;

public:
    void    PlayStageSpin(_int index);

public:
    virtual HRESULT Initialize_Prototype(vector<WEAPON_DESC>* Desc);
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake()                     override;
    virtual void    Priority_Update(_float dt)  override;
    virtual void    Update(_float dt)           override;
    virtual void    Late_Update(_float dt)      override;

private:
    void    Add_StageScreen();
    void    Update_StageEnviroment(_int index);
    void    Set_Stage(GACHA_STAGE eStage);

private:
    class CGachaStageScreen*    m_pScreen = nullptr;
    class CGachaResult*         m_pResult = nullptr;
    
private:    
    vector<WEAPON_DESC>*    m_pResultDesc = nullptr;
    GACHA_STAGE             m_eStage = GACHA_STAGE::BANGBOO;

    _int                    m_iIndex = -1;

public:
    static CGachaStage* Create(vector<WEAPON_DESC>* Desc);
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END