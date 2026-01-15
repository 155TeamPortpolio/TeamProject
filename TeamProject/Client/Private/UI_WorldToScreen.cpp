#include "pch.h"
#include "UI_WorldToScreen.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CUI_WorldToScreen::Initialize_Prototype()
{
	__super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_WorldToScreen::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

void CUI_WorldToScreen::Update(_float dt)
{ 
    Update_WorldToScreen();     

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_WorldToScreen::Update_WorldToScreen()
{
    auto pCameraMgr = CGameInstance::GetInstance()->Get_CameraMgr();
    _float3 vPos = m_vWorldPos;

    Helper::WorldToScreen(vPos, m_vAnchorOffset, *pCameraMgr->Get_ViewMatrix(), *pCameraMgr->Get_ProjMatrix(), _float4(0.f, 0.f, m_WinSize.x, m_WinSize.y));

    //_float fNearDist = 5.f;
    //_float fFarDist = 6.f;
    //_float fMinScale = 0.2f;
    //_float fMaxScale = 1.f;
    //
    //_float fDistance = (_vector3(m_vWorldPos) - _vector3(pCameraMgr->Get_CameraPos())).Length();
    //
    //_float t = (fDistance - fNearDist) / (fFarDist - fNearDist);
    //t = clamp(t, 0.f, 1.f);
    //
    //_float fScale = fMaxScale + (fMinScale - fMaxScale) * t;
    //
    //m_vScale = _float2(fScale, fScale);
}