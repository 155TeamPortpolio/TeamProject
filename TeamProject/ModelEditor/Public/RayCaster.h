#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)
class IRayService;
class CGameObject;
class IInputService;
class IObjectService;
class ITileService;
class IRenderService;
NS_END

NS_BEGIN(ModelEdit)
class CRayCaster :
    public CGameObject
{
private:
    CRayCaster();
    CRayCaster(const CRayCaster& rhs);
    virtual ~CRayCaster() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Awake() override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

public:
    void Render_GUI() override;
public:
    RAY Get_Ray() { return m_tRay; };
private:
    void Create_Ray();

private:
    POINT m_MousePt = {};
    RAY m_tRay = {};
    IRayService* m_pRayManager = { nullptr };
    const _float4x4* m_pViewMat = { nullptr };
    const _float4x4* m_pProjMat = { nullptr };
public:
    static CRayCaster* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();
};
NS_END