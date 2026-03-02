#include "pch.h"
#include "ImageUI.h"
// Engine
#include "Sprite2D.h"
#include "GameInstance.h"
#include "ObjectContainer.h"

HRESULT CImageUI::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    return S_OK;
}

HRESULT CImageUI::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    auto sprite = Get_Component<CSprite2D>();

    sprite->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");
    sprite->Add_Texture(G_GlobalLevelKey, "empty.png");
     
    sprite->Set_Param("ColorTexMode",  {&m_colorTexModeU, "uint",   sizeof(_uint)});
    sprite->Set_Param("ColorTexMix",   {&m_colorTexMix,   "float",  sizeof(_float)});

    sprite->Set_Param("ColorUVUse",    {&m_colorUVUseU,   "uint",   sizeof(_uint)});
    sprite->Set_Param("ColorUVOffset", {&m_colorUVOffset, "float2", sizeof(Vector2)});
    sprite->Set_Param("ColorUVScale",  {&m_colorUVScale,  "float2", sizeof(Vector2)});

    return S_OK;
}

void CImageUI::Update(_float dt)
{
    __super::Update(dt);

    if (!m_colorUVAutoScroll) return;

    m_colorUVOffset += m_colorUVSpeed * dt;
}

void CImageUI::Load(const nlohmann::ordered_json& data)
{
    __super::Load(data);

    auto sprite = Get_Component<CSprite2D>();

    sprite->Change_Texture(0, G_GlobalLevelKey, data.value("textureTag", ""));

    m_colorTextureKey = data.value("colorTexKey", string("empty.png"));
    m_colorTexModeU  = (_uint)data.value("colorTexMode", 0u);
    m_colorTexMix    = (_float)data.value("colorTexMix", 1.f);

    m_colorUVUseU    = (_uint)data.value("colorUVUse", 0u);

    if (data.contains("colorUVOffset"))
    {
        m_colorUVOffset.x = (_float)data["colorUVOffset"][0];
        m_colorUVOffset.y = (_float)data["colorUVOffset"][1];
    }

    if (data.contains("colorUVScale"))
    {
        m_colorUVScale.x = (_float)data["colorUVScale"][0];
        m_colorUVScale.y = (_float)data["colorUVScale"][1];
    }

    m_colorUVAutoScroll = (_bool)data.value("colorUVAutoScroll", false);

    if (data.contains("colorUVSpeed"))
    {
        m_colorUVSpeed.x = (_float)data["colorUVSpeed"][0];
        m_colorUVSpeed.y = (_float)data["colorUVSpeed"][1];
    }

    sprite->Change_Texture(1, G_GlobalLevelKey, m_colorTextureKey);

    sprite->Set_Param("ColorTexMode",  {&m_colorTexModeU, "uint",   sizeof(_uint)});
    sprite->Set_Param("ColorTexMix",   {&m_colorTexMix,   "float",  sizeof(_float)});

    sprite->Set_Param("ColorUVUse",    {&m_colorUVUseU,   "uint",   sizeof(_uint)});
    sprite->Set_Param("ColorUVOffset", {&m_colorUVOffset, "float2", sizeof(Vector2)});
    sprite->Set_Param("ColorUVScale",  {&m_colorUVScale,  "float2", sizeof(Vector2)});
}

CGameObject* CImageUI::Create()
{
    CImageUI* pInstance = new CImageUI();
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CImageUI");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CImageUI::Clone(INIT_DESC* pArg)
{
    CImageUI* pInstance = new CImageUI(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CImageUI");
        Safe_Release(pInstance);
    }
    return pInstance;
}