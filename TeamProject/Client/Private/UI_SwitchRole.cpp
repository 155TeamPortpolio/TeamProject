#include "pch.h"
#include "UI_SwitchRole.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"

void CUI_SwitchRole::Set_Side(SIDE eSide)
{
    switch (eSide)
    {
    case SIDE::LEFT:
        Change_SpriteTexture(SPRITE::MOUSE, "MouseLIcon.png");
        break;
    case SIDE::RIGHT:
        Change_SpriteTexture(SPRITE::MOUSE, "MouseRIcon.png");
        break;
    }
}

void CUI_SwitchRole::Change_RoleIcon(CHARACTER eCharacter)
{
    UI_Active();

    switch (eCharacter)
    {
    case CHARACTER::Corin:
        Change_SpriteTexture(SPRITE::ROLE, "IconRoleCircle09.png");
        break;
    case CHARACTER::JaneDoe:
        Change_SpriteTexture(SPRITE::ROLE, "IconRoleCircle24.png");
        break;
    case CHARACTER::Miyabi:
        Change_SpriteTexture(SPRITE::ROLE, "IconRoleCircle13.png");
        break;
    } 
}

HRESULT CUI_SwitchRole::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    Add_Component<CObjectContainer>();

    return S_OK;
}

HRESULT CUI_SwitchRole::Initialize(INIT_DESC* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("switch_role.json")));
    Cache();

    return S_OK;
}

void CUI_SwitchRole::Awake()
{
}

void CUI_SwitchRole::Update(_float dt)
{
    __super::Update(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_SwitchRole::UI_Active(void* pArg)
{
    Set_Animation(0);
    for (auto& pChild : Get_Component<CObjectContainer>()->Get_Children())
        if (auto pUI = dynamic_cast<CUI_Object*>(pChild))
            pUI->Set_Animation(0);
}

void CUI_SwitchRole::UI_DeActive(void* pArg)
{
}

void CUI_SwitchRole::Cache()
{
    auto pContainer = Get_Component<CObjectContainer>();
    for (_int i = 0; i < ENUM(SPRITE::END); ++i)
    {
        auto pObj = Get_Component<CObjectContainer>()->Find_Descendant(INSTANCENAMES[i]);
        if (!pObj)
            continue;

        m_pIconSprite[i] = pObj->Get_Component<CSprite2D>();
    }
}

void CUI_SwitchRole::Change_SpriteTexture(SPRITE eSprite, const string& strTextureKey)
{
    auto pSprite = m_pIconSprite[ENUM(eSprite)];
    if (!pSprite)
        return;

    pSprite->Change_Texture(0, G_GlobalLevelKey, strTextureKey);
}

CGameObject* CUI_SwitchRole::Create()
{
    CUI_SwitchRole* pInstance = new CUI_SwitchRole();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_SwitchRole");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_SwitchRole::Clone(INIT_DESC* pArg)
{
    CUI_SwitchRole* pInstance = new CUI_SwitchRole(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_SwitchRole");
        Safe_Release(pInstance);
    }
    return pInstance;
}