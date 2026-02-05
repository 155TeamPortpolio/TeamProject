#pragma once
#include "GachaResult.h"

NS_BEGIN(Engine)
class CAnimator3D;
NS_END

NS_BEGIN(Client)

class CGachaAvatar :
    public CGachaResult
{
private:
    enum class ANIMSTATE { START, LOOP, END };

private:
    CGachaAvatar();
    CGachaAvatar(const CGachaAvatar& rhs);
    virtual ~CGachaAvatar() DEFAULT;

public:
    virtual void SetResult(GACHA_RESULT_DESC Desc) override;

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake()                     override;
    virtual void    Priority_Update(_float dt)  override;
    virtual void    Update(_float dt)           override;
    virtual void    Late_Update(_float dt)      override;

private:
    void Update_States();

private:
    CAnimator3D*    m_pAnimator = nullptr;

private:
    ANIMSTATE       m_eAnimState = ANIMSTATE::START;
    string          strLoopAnim = "";
    _bool           m_bRevealEffect = true;

public:
    static CGachaAvatar* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END