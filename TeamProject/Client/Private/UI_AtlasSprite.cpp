#include "pch.h"
#include "UI_AtlasSprite.h"
// Engine
#include "Sprite2D.h"
#include "Texture.h"
#include "ObjectContainer.h"

HRESULT CUI_AtlasSprite::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    Add_Component<CObjectContainer>();
    return S_OK;
}

HRESULT CUI_AtlasSprite::Initialize(INIT_DESC* pArg)
{
    ATLAS_DESC* pDesc = static_cast<ATLAS_DESC*>(pArg);

    __super::Initialize(pArg);

    auto pSprite = Get_Component<CSprite2D>();
    pSprite->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");
    pSprite->ChangePass("SpriteAnimation");

    if (pDesc)
    {
        m_textureKey = pDesc->textureKey;
        m_frameCountX = pDesc->frameCountX;
        m_frameCountY = pDesc->frameCountY;
        m_frameIndex = pDesc->frameIndex;
        m_heightPx = pDesc->heightPx;
    }

    if (!m_textureKey.empty())
        pSprite->Change_Texture(0, G_GlobalLevelKey, m_textureKey);

    Update_SizeByHeight();
    Apply_Params();

    return S_OK;
}

void CUI_AtlasSprite::Update(_float dt)
{
    __super::Update(dt);
    Apply_Params();
    Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_AtlasSprite::Set_Atlas(const string& textureKey, _uint frameCountX, _uint frameCountY)
{
    m_textureKey = textureKey;
    m_frameCountX = max(1u, frameCountX);
    m_frameCountY = max(1u, frameCountY);

    Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, m_textureKey);

    Update_SizeByHeight();
    Apply_Params();
}

void CUI_AtlasSprite::Set_FrameIndex(_uint frameIndex)
{
    m_frameIndex = frameIndex;
    Apply_Params();
}

void CUI_AtlasSprite::Set_HeightPx(_float heightPx)
{
    m_heightPx = heightPx;
    Update_SizeByHeight();
}

void CUI_AtlasSprite::Apply_Params()
{
    auto pSprite = Get_Component<CSprite2D>();

    pSprite->Set_Param("Col",        {&m_frameCountX, "uint", sizeof(_uint)});
    pSprite->Set_Param("Row",        {&m_frameCountY, "uint", sizeof(_uint)});
    pSprite->Set_Param("FrameIndex", {&m_frameIndex,  "uint", sizeof(_uint)});
}

void CUI_AtlasSprite::Update_SizeByHeight()
{
    auto pSprite = Get_Component<CSprite2D>();
    auto pTex    = pSprite->Get_Texture(0);

    _float aspect = 1.f;

    if (pTex)
    {
        const _uint2 texSize = pTex->Get_Size();
        const _float cellW   = texSize.x / max(1u, m_frameCountX);
        const _float cellH   = texSize.y / max(1u, m_frameCountY);
        if (cellH > 0.f) aspect = cellW / cellH;
    }

    Set_Size({m_heightPx * aspect, m_heightPx});
}

CGameObject* CUI_AtlasSprite::Create()
{
    auto inst = new CUI_AtlasSprite();
    if (FAILED(inst->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUI_AtlasSprite");
        Safe_Release(inst);
    }
    return inst;
}

CGameObject* CUI_AtlasSprite::Clone(INIT_DESC* pArg)
{
    auto inst = new CUI_AtlasSprite(*this);
    if (FAILED(inst->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUI_AtlasSprite");
        Safe_Release(inst);
    }
    return inst;
}