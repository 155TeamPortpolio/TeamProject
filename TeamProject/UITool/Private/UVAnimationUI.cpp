#include "pch.h"
#include "UVAnimationUI.h"
 
#include "GameInstance.h"
#include "Helper_Func.h"
#include "Sprite2D.h"
#include "UITool_Level.h"

_uint CUVAnimationUI::m_iCount = {};
const string CUVAnimationUI::m_strTypeTag = "UVAnimation";

HRESULT CUVAnimationUI::Initialize_Prototype()
{
    __super::Initialize_Prototype();

	return S_OK;
}

HRESULT CUVAnimationUI::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    Set_OriginTexSize(true);

    auto sprite = Get_Component<CSprite2D>();

    sprite->Link_Shader(G_GlobalLevelKey, "VTX_UI.hlsl");
    sprite->ChangePass("UVAnimation");
    sprite->Set_Param("UVOffset", {&m_vUVOffset, "float2", sizeof(_float2)});

    m_strTextureKey = "empty.png";
    ApplySpriteTexture(0, G_GlobalLevelKey, m_strTextureKey, true);

    m_iCount++;

    return S_OK;
}

void CUVAnimationUI::Update(_float dt)
{
    if (!m_isAlive)
        return;

    Play_Animation(dt);

    m_vUVOffset.x += m_vUVOffsetSpeed.x * dt;
    m_vUVOffset.y += m_vUVOffsetSpeed.y * dt;
    Get_Component<CSprite2D>()->Set_Param("UVOffset", { &m_vUVOffset,"float2",sizeof(_float2) });
}
void CUVAnimationUI::Render_GUI()
{
    __super::Render_GUI();

    Render_GUI_Image(m_strTextureKey);

    ImGui::SeparatorText(u8"UV애니메이션");
    ImGui::DragFloat2(u8"속도", reinterpret_cast<_float*>(&m_vUVOffsetSpeed), 0.01f);
}

void CUVAnimationUI::Save(nlohmann::ordered_json& data)
{
    __super::Save(data);

    data["typeTag"] = m_strTypeTag;
    data["textureTag"] = m_strTextureKey;

    auto& uvAnimationJson = data["uvAnimation"];
    uvAnimationJson["uvOffsetSpeed"] = { m_vUVOffsetSpeed.x, m_vUVOffsetSpeed.y };
}

void CUVAnimationUI::Load(const nlohmann::ordered_json& data)
{
    __super::Load(data);

    m_strTextureKey = data.value("textureTag", "");

    if (data.contains("uvAnimation"))
    {
        const auto& uvAnimationJson = data["uvAnimation"];
        auto uvOffsetSpeed = uvAnimationJson.value("uvOffsetSpeed", json::array({ 0.0f, 0.0f }));
        m_vUVOffsetSpeed = { uvOffsetSpeed[0], uvOffsetSpeed[1] };
    }

    ApplySpriteTexture(0, G_GlobalLevelKey, m_strTextureKey, false);
}

CGameObject* CUVAnimationUI::Create()
{
    CUVAnimationUI* pInstance = new CUVAnimationUI();

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CUVAnimationUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUVAnimationUI::Clone(INIT_DESC* pArg)
{
    CUVAnimationUI* pInstance = new CUVAnimationUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Clone : CUVAnimationUI");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUVAnimationUI::Free()
{
    __super::Free();
}