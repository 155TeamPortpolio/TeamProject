#include "pch.h"
#include "UI_SwitchRT.h"

#include "GameInstance.h"
#include "ObjectContainer.h" 

HRESULT CUI_SwitchRT::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_SwitchRT::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("switchRT.json")));

    Set_Alive(false);

    return S_OK;
}

void CUI_SwitchRT::Awake()
{
    __super::Awake();
}

void CUI_SwitchRT::Update(_float dt)
{
    __super::Update(dt);

    if (Is_AnimFinished())
    {
        Set_RenderTargetScreenRenderLayer(RENDER_LAYER::None);
        Set_Alive(false);
        return;
    } 

    Update_RTV("renderTargetScreen", true);
}

void CUI_SwitchRT::UI_Active(void* pArg)
{
    Set_RenderTargetScreenRenderLayer(RENDER_LAYER::Default);
    Set_Alive(true);
    Set_Animation(0);
    for (auto& pChild : Get_Component<CObjectContainer>()->Get_Children())
        dynamic_cast<CUI_Object*>(pChild)->Set_Animation(0);
}

void CUI_SwitchRT::UI_DeActive(void* pArg)
{
    Set_RenderTargetScreenRenderLayer(RENDER_LAYER::None);
}

CGameObject* CUI_SwitchRT::Create()
{
    CUI_SwitchRT* pInstance = new CUI_SwitchRT();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_SwitchRT");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_SwitchRT::Clone(INIT_DESC* pArg)
{
    CUI_SwitchRT* pInstance = new CUI_SwitchRT(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_SwitchRT");
        Safe_Release(pInstance);
    }
    return pInstance;
}