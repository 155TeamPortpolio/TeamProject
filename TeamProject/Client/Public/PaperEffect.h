#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)
class CMaterialInstance;
NS_END

NS_BEGIN(Client)

class CPaperEffect :
    public CGameObject
{
public:
    typedef struct tagPaperEffect{
        _vector3 vPosition{};
        _vector3 vVelocity{};

        _vector3 vRotation{};
        _vector3 vAngularSpeed{};

        _vector2 fLifeTime{};
        _vector2 vUV{};

        _float fScale{};

        _float fWindForce{};
        _float fGravityScale{};
    }PAPEREFFECT;

private:
    CPaperEffect();
    CPaperEffect(const CPaperEffect& rhs);
    virtual ~CPaperEffect() DEFAULT;

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
    CMaterialInstance* m_pMaterial_Instance = { nullptr };

    _uint m_ParticleCount = {};
    
    vector<INSTANCE_INIT_DESC> m_InitDesc;
    
    vector<PAPEREFFECT>        m_PaperEffect;
    vector<INSTANCE_PAPER>     m_InstancePaper;

public:
    static CPaperEffect* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END