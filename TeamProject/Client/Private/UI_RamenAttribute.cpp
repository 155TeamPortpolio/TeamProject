#include "pch.h"
#include "UI_RamenAttribute.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"

HRESULT CUI_RamenAttribute::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_RamenAttribute::Initialize(INIT_DESC* pArg)
{
    ATTRIBUTE_DESC* pDesc = static_cast<ATTRIBUTE_DESC*>(pArg);

	__super::Initialize(pArg);

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("ramen_attribute.json")));

    auto pContainer = Get_Component<CObjectContainer>();
    auto pObj = pContainer->Find_Descendant("icon");
    if (pObj)
        m_pIconSprite = pObj->Get_Component<CSprite2D>();

    if (m_pIconSprite)
    {
        auto iter = ICON_TEXTURES.find(pDesc->strAttributeID);
        if(iter != ICON_TEXTURES.end())
            m_pIconSprite->Change_Texture(0, G_GlobalLevelKey,  iter->second);
    } 

	return S_OK;
}

void CUI_RamenAttribute::Awake()
{
}

void CUI_RamenAttribute::Update(_float dt)
{
	__super::Update(dt);

	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_RamenAttribute::Late_Update(_float dt)
{
}

void CUI_RamenAttribute::UI_Active(void* pArg)
{
}

void CUI_RamenAttribute::UI_DeActive(void* pArg)
{
}

CGameObject* CUI_RamenAttribute::Create()
{
    CUI_RamenAttribute* pInstance = new CUI_RamenAttribute();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_RamenAttribute");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_RamenAttribute::Clone(INIT_DESC* pArg)
{
    CUI_RamenAttribute* pInstance = new CUI_RamenAttribute(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_RamenAttribute");
        Safe_Release(pInstance);
    }
    return pInstance;
}