#pragma once
#include "GameObject.h"

NS_BEGIN(Client)
class CAmbientActor abstract :
    public CGameObject
{
public:
    typedef struct tagAmbientActorDesc : public Engine::GAMEOBJECT_DESC {
        string  strAnimName{};
        _bool   bLoop = { false };
    }AMBIENTACTOR_DESC;

protected:
    CAmbientActor();
    CAmbientActor(const CAmbientActor& rhs);
    virtual ~CAmbientActor() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    void            Awake()                     override {};
    virtual void    Priority_Update(_float dt)  override {};
    virtual void    Update(_float dt)           override {};
    virtual void    Late_Update(_float dt)      override {};

protected:
    string  m_AnimName = {};
    _bool   m_bLoop = { false };

public:
    virtual CGameObject* Clone(INIT_DESC* pArg) PURE;
    virtual void Free() override;
};
NS_END