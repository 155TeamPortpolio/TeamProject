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
    ATLAS_DESC* desc = static_cast<ATLAS_DESC*>(pArg);

    __super::Initialize(pArg);

    auto sprite = Get_Component<CSprite2D>();
    sprite->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");
    sprite->ChangePass("SpriteAnimation");

    if (desc)
    {
        m_texKey      = desc->texKey;
        m_frameCountX = desc->frameCountX;
        m_frameCountY = desc->frameCountY;
        m_frameIdx    = desc->frameIdx;
        m_heightPx    = desc->heightPx;
    }

    if (!m_texKey.empty())
        sprite->Change_Texture(0, G_GlobalLevelKey, m_texKey);

    m_colorTexKey.clear();
    m_colorFrameCountX = 1;
    m_colorFrameCountY = 1;
    m_colorFrameIdx    = 0;

    m_shearK = 0.f;

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
    m_texKey      = textureKey;
    m_frameCountX = max(1u, frameCountX);
    m_frameCountY = max(1u, frameCountY);

    Get_Component<CSprite2D>()->Change_Texture(0, G_GlobalLevelKey, m_texKey);

    Update_SizeByHeight();
    Apply_Params();
}

void CUI_AtlasSprite::Set_FrameIndex(_uint frameIdx)
{
    m_frameIdx = frameIdx;
    Apply_Params();
}

void CUI_AtlasSprite::Set_HeightPx(_float heightPx)
{
    m_heightPx = heightPx;
    Update_SizeByHeight();
}

void CUI_AtlasSprite::Set_ColorAtlas(const string& textureKey, _uint frameCountX, _uint frameCountY)
{
    m_colorTexKey      = textureKey;
    m_colorFrameCountX = max(1u, frameCountX);
    m_colorFrameCountY = max(1u, frameCountY);

    auto sprite = Get_Component<CSprite2D>();
    sprite->Change_Texture(1, G_GlobalLevelKey, m_colorTexKey);
    sprite->ChangePass("SpriteAnimation_ColorAtlas");

    Apply_Params();
}

void CUI_AtlasSprite::Set_ColorFrameIndex(_uint frameIdx)
{
    m_colorFrameIdx = frameIdx;
    Apply_Params();
}

void CUI_AtlasSprite::Set_UseColorAtlas(_bool enable)
{
    m_useColorAtlas = enable ? 1u : 0u;
    Apply_Params();
}

void CUI_AtlasSprite::Set_ShearK(_float k)
{
    m_shearK = k;
    Apply_Params();
}

void CUI_AtlasSprite::Apply_Params()
{
    auto sprite = Get_Component<CSprite2D>();

    if (!sprite->IsValid()) return;

    sprite->Set_Param("Col",        {&m_frameCountX, "uint",  sizeof(_uint)});
    sprite->Set_Param("Row",        {&m_frameCountY, "uint",  sizeof(_uint)});
    sprite->Set_Param("FrameIndex", {&m_frameIdx,    "uint",  sizeof(_uint)});
    sprite->Set_Param("ShearK",     {&m_shearK,      "float", sizeof(_float)});

    if (!m_colorTexKey.empty())
    {
        sprite->Set_Param("ColorCol",        {&m_colorFrameCountX, "uint", sizeof(_uint)});
        sprite->Set_Param("ColorRow",        {&m_colorFrameCountY, "uint", sizeof(_uint)});
        sprite->Set_Param("ColorFrameIndex", {&m_colorFrameIdx,    "uint", sizeof(_uint)});

        SHADER_PARAM p{};
        p.typeName = "Texture2D";
        p.iSize    = 0;
        p.pData    = sprite->Get_Texture(1)->Get_SRV();
        sprite->Set_Param("ColorTexture", p);
    }
}

void CUI_AtlasSprite::Update_SizeByHeight()
{
    auto sprite = Get_Component<CSprite2D>();
    if (!sprite->IsValid()) return;

    auto tex = sprite->Get_Texture(0);
    _float aspect = 1.f;
    if (tex)
    {
        const _uint2 texSize = tex->Get_Size();
        const _float cellW   = texSize.x / max(1u, m_frameCountX);
        const _float cellH   = texSize.y / max(1u, m_frameCountY);
        if (cellH > 0.f) aspect = cellW / cellH;
    }

    Set_Size({m_heightPx * aspect, m_heightPx});
}

void CUI_AtlasSprite::OnPooledAcquire(INIT_DESC* pArg)
{
    Initialize(pArg);
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