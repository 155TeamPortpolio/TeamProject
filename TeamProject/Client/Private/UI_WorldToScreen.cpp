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
    _bool isValid = Update_WorldToScreenPos(vPosition);
    Update_Visibility(isValid);

    if (m_isValid)
        Update_ZPriority(vPosition);
}

_bool CUI_WorldToScreen::Update_WorldToScreenPos(_float3 vPosition)
{
    auto pCameraMgr = CGameInstance::GetInstance()->Get_CameraMgr();

    return Helper::WorldToScreen(vPosition, m_vAnchorOffset, *pCameraMgr->Get_ViewMatrix(), *pCameraMgr->Get_ProjMatrix(), _float4(0.f, 0.f, m_WinSize.x, m_WinSize.y));
}

void CUI_WorldToScreen::Update_Visibility(_bool isValid)
{
    if (isValid == m_isValid)
        return;

    m_isValid = isValid;
    SetRenderLayer(isValid ? RENDER_LAYER::Default : RENDER_LAYER::None);
}

void CUI_WorldToScreen::Update_ZPriority(_float3 vPosition)
{
    if (!m_isValid)
        return;

    auto pCameraMgr = CGameInstance::GetInstance()->Get_CameraMgr();

    // 플레이어와 월드 위치 사이의 방향 벡터와 카메라 룩을 내적해서 깊이 구하기
    auto pPlayer = ObjectManager()->Find_Global(ENUM(GLOBAL_ID::Player));
    Vector4 vPlayerPos = static_cast<CPlayer*>(pPlayer)->Get_CurCharacterHandle().Get()->Get_Position();
    Vector3 vDiff = (Vector3(vPosition) - Vector3(vPlayerPos));
    vDiff.Normalize();
    _float fDot = vDiff.Dot(Vector3(pCameraMgr->GetForward()));
    m_Zpriority = fDot + 1.f;   // 플레이어랑 내적해서 구하기 때문에 화면상에서 테두리쪽에 있을 때 HUD 위에 그려져서 보정 값으로 1.f 더함
}