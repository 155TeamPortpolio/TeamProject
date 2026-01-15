#include "pch.h"
#include "UI_IconLabel.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CUI_IconLabel::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CUI_IconLabel::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);
    
    auto pResourceMgr = CGameInstance::GetInstance()->GetInstance()->Get_ResourceMgr();
    const string& filePath = pResourceMgr->Get_ResourcePath(static_cast<UI_DESC*>(pArg)->UIAssetKey);
    Load(Helper::LoadJson<nlohmann::ordered_json>(filePath));

    return S_OK;
}

void CUI_IconLabel::Update(_float dt)
{
    __super::Update(dt);
}

CGameObject* CUI_IconLabel::Create()
{
    CUI_IconLabel* pInstance = new CUI_IconLabel();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_IconLabel");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_IconLabel::Clone(INIT_DESC* pArg)
{
    CUI_IconLabel* pInstance = new CUI_IconLabel(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_IconLabel");
        Safe_Release(pInstance);
    }
    return pInstance;
}