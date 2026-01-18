#include "pch.h"
#include "UI_Logo.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CUI_Logo::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CUI_Logo::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    // JSON 기반 UI 구성 로드
    const string& filePath = ResourceManager()->Get_ResourcePath("logo.json");
    Load(Helper::LoadJson<nlohmann::ordered_json>(filePath));

    return S_OK;
}

void CUI_Logo::Awake()
{
    // 0번 애니메이션 재생 (FadeIn)
    Set_Animation(0);
}

void CUI_Logo::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

CGameObject* CUI_Logo::Create()
{
    CUI_Logo* pInstance = new CUI_Logo();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_Logo");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Logo::Clone(INIT_DESC* pArg)
{
    CUI_Logo* pInstance = new CUI_Logo(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_Logo");
        Safe_Release(pInstance);
    }
    return pInstance;
}