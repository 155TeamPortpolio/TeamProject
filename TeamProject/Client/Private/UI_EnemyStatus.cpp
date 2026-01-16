#include "pch.h"
#include "UI_EnemyStatus.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Child.h"

HRESULT CUI_EnemyStatus::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CUI_EnemyStatus::Initialize(INIT_DESC* pArg)
{
    ENEMYSTATUS_DESC* pDesc = static_cast<ENEMYSTATUS_DESC*>(pArg);
    m_pParentWorld = pDesc->pParentWorld;
    m_pBoneLocal = pDesc->pBoneLocal;
    m_pMonsterStatus = pDesc->pMonsterStatus;

    __super::Initialize(pArg);

    auto pResourceMgr = CGameInstance::GetInstance()->GetInstance()->Get_ResourceMgr();
    const string& filePath = pResourceMgr->Get_ResourcePath("enemy_status.json");
    Load(Helper::LoadJson<nlohmann::ordered_json>(filePath));


    return S_OK;
}

void CUI_EnemyStatus::Update(_float dt)
{ 
    if (!m_pParentWorld || !m_pBoneLocal)
        return;
    
    Matrix matWorld = *m_pBoneLocal * *m_pParentWorld;
    
    _float3 vPosition = { matWorld.m[3][0], matWorld.m[3][1], matWorld.m[3][2] };
    
    Update_WorldToScreen(vPosition);
     
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
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