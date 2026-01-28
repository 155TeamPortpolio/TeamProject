#include "pch.h"
#include "UI_Newspaper.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"

HRESULT CUI_Newspaper::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_Newspaper::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Load(Helper::LoadJson<nlohmann::ordered_json>(ResourceManager()->Get_ResourcePath("newspaper.json")));

    Cache();

    // 자식 뉴스 텍스쳐 초기 셋팅
    for (_int i = 0; i < ENUM(CHILD::END); ++i)
    {
        Change_ChildTexture(static_cast<CHILD>(i), TEXTURES[m_iTextureIndex]);
        m_iTextureIndex = (m_iTextureIndex + 1) % TEXTURE_COUNT;
    }
    
	return S_OK;
}

void CUI_Newspaper::Awake()
{
}

void CUI_Newspaper::Update(_float dt)
{
    __super::Update(dt);

    Update_Paging(dt);

    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_Newspaper::UI_Active(void* pArg)
{
    m_isPaging = true;
    Set_ChildAnimation(static_cast<CHILD>(m_iChildIndex), 0);
}

void CUI_Newspaper::UI_DeActive(void* pArg)
{
}

void CUI_Newspaper::Cache()
{
    auto pContainer = Get_Component<CObjectContainer>();

    for (_int i = 0; i < ENUM(CHILD::END); ++i)
    {
        auto pObj = pContainer->Find_Descendant(INSTANCENAMAES[i]);
        if (!pObj)
            continue;

        m_pChildren[i] = dynamic_cast<CUI_Object*>(pObj);
        m_pSprites[i] = pObj->Get_Component<CSprite2D>();
    }
}

void CUI_Newspaper::Update_Paging(_float dt)
{
    if (!m_isPaging)
        return;

    m_fPagingTimer += dt;
    
    if (m_fPagingTimer <= m_fPagingDuration)
        return;

    m_fPagingTimer = 0.f;
    m_isPaging = false;
    
    Get_Component<CObjectContainer>()->Upper_Order(m_pChildren[m_iChildIndex]);
    Change_ChildTexture(static_cast<CHILD>(m_iChildIndex), TEXTURES[m_iTextureIndex]);
    
    m_iTextureIndex = (m_iTextureIndex + 1) % TEXTURE_COUNT;
    m_iChildIndex = (m_iChildIndex + 1) % ENUM(CHILD::END);
}

void CUI_Newspaper::Set_ChildAnimation(CHILD child, _int iIndex)
{
    auto pChild = m_pChildren[ENUM(child)];
    if (!pChild)
        return;

    pChild->Set_Animation(iIndex);
}

void CUI_Newspaper::Change_ChildTexture(CHILD child, const string& strTextureKey)
{
    auto pSprite = m_pSprites[ENUM(child)];
    if (!pSprite)
        return;

    pSprite->Change_Texture(0, G_GlobalLevelKey, strTextureKey); 
}

CGameObject* CUI_Newspaper::Create()
{
    CUI_Newspaper* pInstance = new CUI_Newspaper();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_Newspaper");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Newspaper::Clone(INIT_DESC* pArg)
{
    CUI_Newspaper* pInstance = new CUI_Newspaper(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_Newspaper");
        Safe_Release(pInstance);
    }
    return pInstance;
}