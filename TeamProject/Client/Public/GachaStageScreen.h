#pragma once
#include "GameObject.h"

NS_BEGIN(Client)
class CGachaStage;
class CGachaStageScreen :
    public CGameObject
{
private:
    CGachaStageScreen();
    CGachaStageScreen(const CGachaStageScreen& rhs);
    virtual ~CGachaStageScreen() DEFAULT;

public:
    void SetScreen(GACHA_STAGE eStage, GachaGrade eGrade);

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake()                     override;
    virtual void    Priority_Update(_float dt)  override;
    virtual void    Update(_float dt)           override;
    virtual void    Late_Update(_float dt)      override;

private:
    void ResetMaterialInstances(vector<_int> ScreenIndex);

private:
    vector<_int>    m_Cols = { 4 };
    vector<_int>    m_Rows = { 8 };
    vector<_int>    m_CurrentFrameIndexs = { 0 };
    vector<_int>    m_MaxFrameIndexs = { 28 };
    _float          m_fElapsedTime = 0.f;
    _float          m_fFrameDuration = 0.02f;

    _int            m_iMaterialInstanceCounts;

public:
    static CGachaStageScreen* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END