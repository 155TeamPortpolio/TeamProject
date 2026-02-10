#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)
class CMaterialInstance;
NS_END

NS_BEGIN(Client)

class CXWall :
    public CGameObject
{
private:
    CXWall();
    CXWall(const CXWall& rhs);
    virtual ~CXWall() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;
    void    Awake() override;

public:
    virtual void Render_GUI() override;

private:
    _vector2 m_vCount = { 10, 3 };
    _vector2 m_vOffset = { 5.f, 3.f };
    CMaterialInstance* m_pMaterial_Instance = { nullptr };

    vector<INSTANCE_INIT_DESC> m_InitDescs;
    vector<INSTANCE_XWALL> m_XWall;
public:
    static CXWall* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END