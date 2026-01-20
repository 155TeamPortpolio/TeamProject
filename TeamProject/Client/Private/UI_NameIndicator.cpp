#include "pch.h"
#include "UI_NameIndicator.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CUI_NameIndicator::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CUI_NameIndicator::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    const string& filePath = ResourceManager()->Get_ResourcePath("name_indicator");
    Load(Helper::LoadJson<nlohmann::ordered_json>(filePath));

    return S_OK;
}

void CUI_NameIndicator::Update(_float dt)
{
    Update_WorldToScreen(m_vWorldPos);

    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

CGameObject* CUI_NameIndicator::Create()
{
    CUI_NameIndicator* pInstance = new CUI_NameIndicator();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_NameIndicator");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_NameIndicator::Clone(INIT_DESC* pArg)
{
    CUI_NameIndicator* pInstance = new CUI_NameIndicator(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_NameIndicator");
        Safe_Release(pInstance);
    }
    return pInstance;
}