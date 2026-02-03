#include "pch.h"
#include "UI_GachaChannel.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CUI_GachaChannel::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_GachaChannel::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("gacha_channel.json")));

	return S_OK;
}

void CUI_GachaChannel::Awake()
{
}

void CUI_GachaChannel::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_GachaChannel::UI_Active(void* pArg)
{
}

void CUI_GachaChannel::UI_DeActive(void* pArg)
{
}

CGameObject* CUI_GachaChannel::Create()
{
    CUI_GachaChannel* pInstance = new CUI_GachaChannel();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_GachaChannel");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_GachaChannel::Clone(INIT_DESC* pArg)
{
    CUI_GachaChannel* pInstance = new CUI_GachaChannel(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_GachaChannel");
        Safe_Release(pInstance);
    }
    return pInstance;
}