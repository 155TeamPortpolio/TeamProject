#pragma once
#include "AmbientActor.h"

NS_BEGIN(Client)

class CScottScreen final :
    public CAmbientActor
{
private:
    typedef struct tagScreenColRow {
        _int iCol{}, iRow{}, iMaxFrame{};
        _int iCurrentFrameIndex{};
    }SCREEN;

private:
    CScottScreen();
    CScottScreen(const CScottScreen& rhs);
    virtual ~CScottScreen() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    void            Awake() override;
    virtual void    Priority_Update(_float dt)  override;
    virtual void    Update(_float dt)           override;
    virtual void    Late_Update(_float dt)      override;

private:
    _uint  m_iScreenCount = { 6 };
    SCREEN m_pScreen[6];
    _float m_fFrameDuration = { 0.01f };
    _float m_fFrameElapsedTime = { 0.f };

public:
    static CScottScreen* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END