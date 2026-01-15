#include "pch.h"
#include "UI_EnemyStatus.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CUI_EnemyStatus::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CUI_EnemyStatus::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    auto pResourceMgr = CGameInstance::GetInstance()->GetInstance()->Get_ResourceMgr();
    const string& filePath = pResourceMgr->Get_ResourcePath("enemy_status");
    Load(Helper::LoadJson<nlohmann::ordered_json>(filePath));

    return S_OK;
}

void CUI_EnemyStatus::Update(_float dt)
{
    __super::Update(dt);
}

CGameObject* CUI_EnemyStatus::Create()
{
    CUI_EnemyStatus* pInstance = new CUI_EnemyStatus();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_EnemyStatus");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_EnemyStatus::Clone(INIT_DESC* pArg)
{
    CUI_EnemyStatus* pInstance = new CUI_EnemyStatus(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_EnemyStatus");
        Safe_Release(pInstance);
    }
    return pInstance;
}