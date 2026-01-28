#pragma once
#include "GameObject.h"
#include "ICamCollidable.h"

NS_BEGIN(Client)
class CNpc abstract:
    public CGameObject, public ICamCollidable
{
protected:
    CNpc();
    CNpc(const CNpc& rhs);
    virtual ~CNpc() DEFAULT;

public:
    virtual void    Execute() {};

public:
    const string&  Get_AnimName() const { return m_strAnimName; }
    const wstring& Get_Name() const { return m_strName; }
    virtual OBJECT_HANDLE Get_PartnerHandle() { return OBJECT_HANDLE{}; }

public:
    virtual void OnCameraCollision(_bool isColliding) override;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;

protected:
    HRESULT         Add_NameIndicator();
    HRESULT         Add_InteractZonePrototype();
    HRESULT         Add_InteractZone(_float4 vCenter, _float3 vOffset,
        _float3 vRotate, _float3 vSize = _float3(1.f,1.f,1.f));

protected:
    string m_strAnimName = "";
    wstring m_strName = TEXT("");
    _float m_fCamFadeAlpha = 0.f;

public:
    virtual CGameObject* Clone(INIT_DESC* pArg) PURE;
    virtual void Free() override;
};
NS_END