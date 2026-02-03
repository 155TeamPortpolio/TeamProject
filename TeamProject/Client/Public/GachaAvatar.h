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
    CGachaAvatar();
    CGachaAvatar(const CGachaAvatar& rhs);
    virtual ~CGachaAvatar() DEFAULT;

public:
    virtual void SetResult(string strModel, string strMaterial, _float4 vRot) override;

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake()                     override;
    virtual void    Priority_Update(_float dt)  override;
    virtual void    Update(_float dt)           override;
    virtual void    Late_Update(_float dt)      override;

private:
    CAnimator3D*    m_pAnimator = nullptr;

public:
    static CGachaAvatar* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END