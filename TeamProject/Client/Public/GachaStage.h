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
    virtual HRESULT Initialize_Prototype(vector<WEAPON_DESC>* Desc, _int* Index);
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake()                     override;
    virtual void    Priority_Update(_float dt)  override;
    virtual void    Update(_float dt)           override;
    virtual void    Late_Update(_float dt)      override;

private:
    void    Add_StageScreen();
    void    Update_StageEnviroment();
    void    Set_Stage(GACHA_STAGE eStage);

private:
    class CGachaStageScreen*    m_pScreen = nullptr;
    class CGachaResult*         m_pResult = nullptr;
    
private:    
    vector<WEAPON_DESC>*    m_pResultDesc = nullptr;
    _int*                   m_pIndex = nullptr;
    _int                    m_iPreIndex = -1;
    GACHA_STAGE             m_eStage = GACHA_STAGE::BANGBOO;

public:
    static CGachaStage* Create(vector<WEAPON_DESC>* Desc, _int* Index);
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END