#include "pch.h"
#include "UI_AccessSign.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CUI_AccessSign::Initialize_Prototype()
{
	__super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_AccessSign::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	auto pResourceMgr = CGameInstance::GetInstance()->GetInstance()->Get_ResourceMgr();
	const string& filePath = pResourceMgr->Get_ResourcePath("accessSign_zero.json");
	Load(Helper::LoadJson<nlohmann::ordered_json>(filePath));

	return S_OK;
}

void CUI_AccessSign::Awake()
{
}

void CUI_AccessSign::Update(_float dt)
{
    _vector3 vPos = {};

    auto pGameInstance = CGameInstance::GetInstance();
    auto pCameraMgr = pGameInstance->Get_CameraMgr();
    _float2 vSize = pGameInstance->Get_ClientSize();
    Helper::WorldToScreen(vPos, m_vAnchorOffset, *pCameraMgr->Get_ViewMatrix(), *pCameraMgr->Get_ProjMatrix(), _float4(0.f, 0.f, vSize.x, vSize.y));
    _vector3 vSub = vPos - pCameraMgr->Get_CameraPos();

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

CGameObject* CUI_AccessSign::Create()
{
    CUI_AccessSign* pInstance = new CUI_AccessSign();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_AccessSign");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_AccessSign::Clone(INIT_DESC* pArg)
{
    CUI_AccessSign* pInstance = new CUI_AccessSign(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_AccessSign");
        Safe_Release(pInstance);
    }
    return pInstance;
}