#pragma once
#include "GameObject.h"

NS_BEGIN(Client)

class CKitObject final :
    public CGameObject
{
public:
    typedef struct tagKitDesc : public Engine::GAMEOBJECT_DESC {
        _uint iItemType{};
    }KIT_DESC;

private:
    CKitObject();
    CKitObject(const CKitObject& rhs);
    virtual ~CKitObject() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    void            Awake() override;
    virtual void    Priority_Update(_float dt)  override;
    virtual void    Update(_float dt)           override;
    virtual void    Late_Update(_float dt)      override;

    virtual void Render_GUI()override;

private:
    _vector3 m_vDegreePerSec = { 0.f, 30.f, 0.f };

public:
    static CKitObject* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END