#include "pch.h"
#include "UI_WorldToScreen.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

#include "CameraMgr.h"
#include "Player.h"

HRESULT CUI_WorldToScreen::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CUI_WorldToScreen::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	return S_OK;
}

void CUI_WorldToScreen::Update(_float dt)
{   
    __super::Update(dt);
}

void CUI_WorldToScreen::Update_WorldToScreen(_float3 vPosition)
{
    auto pCameraMgr = CGameInstance::GetInstance()->Get_CameraMgr();
    
    // 월드 위치로 스크린 위치 구하기
    Helper::WorldToScreen(vPosition, m_vAnchorOffset, *pCameraMgr->Get_ViewMatrix(), *pCameraMgr->Get_ProjMatrix(), _float4(0.f, 0.f, m_WinSize.x, m_WinSize.y));

    // 플레이어와 월드 위치 사이의 방향 벡터와 카메라 룩을 내적해서 깊이 구하기
    auto pPlayer = ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Player));
    Vector4 vPlayerPos = static_cast<CPlayer*>(pPlayer)->Get_CurCharacterHandle().Get()->Get_Position();
    Vector3 vDiff = (Vector3(vPosition) - Vector3(vPlayerPos));
    vDiff.Normalize();
    _float fDot = vDiff.Dot(Vector3(pCameraMgr->GetForward()));
    m_Zpriority = fDot;

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