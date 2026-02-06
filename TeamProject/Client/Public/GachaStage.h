#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CGachaStage :
    public CGameObject
{
private:
    typedef struct StageMainLight
    {
        _vector4                 StartColor;
        _vector4                 MiddleColor;
        _vector4                 EndColor;
        _float                  CurElpasedTime = 0.f;
        _float                  Duration;
    }STAGE_LIGHT;

private:
    CGachaStage();
    CGachaStage(const CGachaStage& rhs);
    virtual ~CGachaStage() DEFAULT;

public:
    void    PlayStageSpin(_int index);
    void    PlayRevealEffect();

public:
    virtual HRESULT Initialize_Prototype(vector<GACHA_RESULT_DESC>* Desc);
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake()                     override;
    virtual void    Priority_Update(_float dt)  override;
    virtual void    Update(_float dt)           override;
    virtual void    Late_Update(_float dt)      override;

private:
    void    Add_StageScreen();
    void    Update_StageEnviroment(_int index);
    void    Set_Stage(GACHA_STAGE eStage, _int ResultID);

    void    Update_CamTime();
    void    Update_Lights(_float dt);

    void    SetBottomLightEffect(_float4 BottomStartColor, _float4 BottomMiddleColor, _float4 BottomEndColor, _float Duration);
    void    SetTopLightEffect(_float4 TopStartColor, _float4 TopMiddleColor, _float4 TopEndColor, _float Duration);
    void    SetMiddleLightEffect(_float4 MiddleStartColor, _float4 MiddleEndColor, _float Duration);
    void    SetInitLight();

private:
    class CGachaStageScreen*    m_pScreen = nullptr;
    class CGachaResult*         m_pWeaponResult = nullptr;
    class CGachaResult*         m_pAvatarResult = nullptr;
    class CGachaFootStage*      m_pFootStage = nullptr;
    OBJECT_HANDLE               m_MainTopLightHandle = {};
    OBJECT_HANDLE               m_MainBottomLightHandle = {};
    OBJECT_HANDLE               m_MainMiddleLightHandle = {};

private:    
    vector<GACHA_RESULT_DESC>*    m_pResultDesc = nullptr;
    GACHA_STAGE             m_eStage = GACHA_STAGE::BANGBOO;

    _int                    m_iIndex = -1;
    _int                    m_iSpinIndex = -1;
    _int					m_iMaxIndex = 10;

    STAGE_LIGHT             BottomLight;
    STAGE_LIGHT             TopLight;
    STAGE_LIGHT             MiddleLight;

public:
    static CGachaStage* Create(vector<GACHA_RESULT_DESC>* Desc);
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END