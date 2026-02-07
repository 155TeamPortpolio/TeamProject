#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CUI_GachaText : public CGameObject
{
public:
    typedef struct tagTextDesc : public GAMEOBJECT_DESC {
        _vector vScaleOffset;
        _vector vPosOffset; 
    }TEXT_DESC;

private:
    CUI_GachaText() {}
    CUI_GachaText(const CUI_GachaText& rhs) : CGameObject(rhs) {}
    virtual ~CUI_GachaText() DEFAULT;

public:
    void Play(const string& strKey);

    void Change_Model(const string& strKey);

public:
    HRESULT Initialize_Prototype()     override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Priority_Update(_float dt) override {}
    void    Update(_float dt)          override;
    void    Late_Update(_float dt)     override {}

private:
    _bool m_isMoving = {};

    _float m_fTimer = {};
    _float m_fDuration = { 1.2f };

    _vector m_fInitScale = {};
    _vector m_fTargetScale = {};

    _vector m_fInitPos = {};
    _vector m_fTargetPos = {};
     
private:
    void Restart_Move();

    void Update_Move(_float dt);

public:
    static CGameObject* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override { __super::Free(); }
};

NS_END