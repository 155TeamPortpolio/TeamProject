#include "pch.h"
#include "UI_Wipeout.h"

#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CUI_Wipeout::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_Wipeout::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("wipeout.json")));

    Set_Alpha(m_isVisible? 1.f : 0.f);

	return S_OK;
}

void CUI_Wipeout::Awake()
{
}

void CUI_Wipeout::Update(_float dt)
{
    if (InputDevice()->Key_Tap('U'))
    {
        m_isVisible = !m_isVisible;
        Set_Alpha(m_isVisible ? 1.f : 0.f);
    }

    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

CGameObject* CUI_Wipeout::Create()
{
    CUI_Wipeout* pInstance = new CUI_Wipeout();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_Wipeout");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Wipeout::Clone(INIT_DESC* pArg)
{
    CUI_Wipeout* pInstance = new CUI_Wipeout(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_Wipeout");
        Safe_Release(pInstance);
    }
    return pInstance;
}