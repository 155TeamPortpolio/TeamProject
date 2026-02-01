#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CGachaStageScreen :
    public CGameObject
{
private:
    CGachaStageScreen();
    CGachaStageScreen(const CGachaStageScreen& rhs);
    virtual ~CGachaStageScreen() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake()                     override;
    virtual void    Priority_Update(_float dt)  override;
    virtual void    Update(_float dt)           override;
    virtual void    Late_Update(_float dt)      override;

private:
    _int    m_iCol = 8;
    _int    m_iRow = 5;
    _int    m_iCurrentFrameIndex = 0;
    _int    m_iMaxFrameIndex = 34;
    _float  m_fElapsedTime = 0.f;
    _float  m_fFrameDuration = 0.02f;

public:
    static CGachaStageScreen* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END