#include "pch.h"
#include "UI_clear.h"

#include "GameInstance.h"
#include "ObjectContainer.h" 

HRESULT CUI_Clear::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Clear::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("clear.json")));

    Set_Alive(false);

    return S_OK;
}

void CUI_Clear::Awake()
{
    __super::Awake();
}

void CUI_Clear::Update(_float dt)
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

void CUI_Clear::UI_Active(void* pArg)
{
    Set_RenderTargetScreenRenderLayer(RENDER_LAYER::Default);
    Set_Alive(true);
    Set_Animation(0);
    for (auto& pChild : Get_Component<CObjectContainer>()->Get_Children())
        dynamic_cast<CUI_Object*>(pChild)->Set_Animation(0);
}

CGameObject* CUI_Clear::Create()
{
    CUI_Clear* pInstance = new CUI_Clear();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_Clear");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Clear::Clone(INIT_DESC* pArg)
{
    CUI_Clear* pInstance = new CUI_Clear(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_Clear");
        Safe_Release(pInstance);
    }
    return pInstance;
}