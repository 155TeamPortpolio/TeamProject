#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CGachaScreen :
    public CGameObject
{
private:
    CGachaScreen();
    CGachaScreen(const CGachaScreen& rhs);
    virtual ~CGachaScreen() DEFAULT;

public:
    void PlayTVSequence(vector<WEAPON_DESC>* ResultDesc);
    void SetupInitialTVSequence(vector<WEAPON_DESC>* ResultDesc);

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake()                     override;
    virtual void    Priority_Update(_float dt)  override;
    virtual void    Update(_float dt)           override;
    virtual void    Late_Update(_float dt)      override;

private:
    void SetMaterialInstances(_int ChangeNum, vector<_int> ScreenIndex);

private:
    vector<_int>    m_Cols = {4};
    vector<_int>    m_Rows = {8};
    vector<_int>    m_CurrentFrameIndexs = {0};
    vector<_int>    m_MaxFrameIndexs = {28};
    _float          m_fFrameElapsedTime = 0.f;
    _float          m_fFrameDuration = 0.02f;

    _int            m_iMaterialInstanceCounts;

    _bool           m_bIsPlaying = false;
    _int            m_iCurPlayingIndex = 0;
    _float          m_fIntervalScreenDuration = 0.15f;
    _float          m_fScreenElapsedTime = 0.f;
public:
    static CGachaScreen* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END