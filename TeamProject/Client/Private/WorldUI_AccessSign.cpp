#include "pch.h"
#include "WorldUI_AccessSign.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CWorldUI_AccessSign::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    return S_OK;
}

HRESULT CWorldUI_AccessSign::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);
    
    auto pResourceMgr = CGameInstance::GetInstance()->GetInstance()->Get_ResourceMgr();
    const string& filePath = pResourceMgr->Get_ResourcePath(static_cast<UI_DESC*>(pArg)->UIAssetKey);
    Load(Helper::LoadJson<nlohmann::ordered_json>(filePath));

    return S_OK;
}

void CWorldUI_AccessSign::Update(_float dt)
{
    __super::Update(dt);
}

CGameObject* CWorldUI_AccessSign::Create()
{
    CWorldUI_AccessSign* pInstance = new CWorldUI_AccessSign();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_AccessSignZero");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CWorldUI_AccessSign::Clone(INIT_DESC* pArg)
{
    CWorldUI_AccessSign* pInstance = new CWorldUI_AccessSign(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CWorldUI_AccessSign");
        Safe_Release(pInstance);
    }
    return pInstance;
}