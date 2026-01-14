#include "pch.h"
#include "WorldUI_Object.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CWorldUI_Object::Initialize_Prototype()
{
	__super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CWorldUI_Object::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

    m_vScreenSize = CGameInstance::GetInstance()->Get_ClientSize();

	return S_OK;
}

void CWorldUI_Object::Update(_float dt)
{ 
    Update_WorldToScreen();     

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CWorldUI_Object::Update_WorldToScreen()
{
    auto pCameraMgr = CGameInstance::GetInstance()->Get_CameraMgr();
    Helper::WorldToScreen(m_vWorldPos, m_vAnchorOffset, *pCameraMgr->Get_ViewMatrix(), *pCameraMgr->Get_ProjMatrix(), _float4(0.f, 0.f, m_vScreenSize.x, m_vScreenSize.y));

    _float fNearDist = 5.f;
    _float fFarDist = 6.f;
    _float fMinScale = 0.2f;
    _float fMaxScale = 1.f;

    _float fDistance = (_vector3(m_vWorldPos) - _vector3(pCameraMgr->Get_CameraPos())).Length();
    
    _float t = (fDistance - fNearDist) / (fFarDist - fNearDist);
    t = clamp(t, 0.f, 1.f);

    _float fScale = fMaxScale + (fMinScale - fMaxScale) * t;

    m_vScale = _float2(fScale, fScale);
}