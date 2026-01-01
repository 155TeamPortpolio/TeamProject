#include "pch.h"
#include "UVAnimationUI.h"
 
#include "GameInstance.h"
#include "Helper_Func.h"
#include "Sprite2D.h"
#include "UITool_Level.h"
#include "Texture.h"

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
    sprite->Set_Param("UVOffset",        {&m_vUVOffset,       "float2", sizeof(_float2)});
    sprite->Set_Param("MaskThreshold",   {&m_maskThreshold,   "float",  sizeof( _float)});
    sprite->Set_Param("MaskSoftness",    {&m_maskSoftness,    "float",  sizeof( _float)});
    sprite->Set_Param("MaskTextureSize", {&m_maskTextureSize, "float2", sizeof(_float2)});

    m_strTextureKey = "empty.png";
    ApplySpriteTexture(0, G_GlobalLevelKey, m_strTextureKey, true);

    m_maskTextureKey = "empty.png";
    sprite->Change_Texture(1, G_GlobalLevelKey, m_maskTextureKey);

    {
        auto maskTexture  = sprite->Get_Texture(1);
        auto size         = maskTexture->Get_Size();
        m_maskTextureSize = {(float)size.x, (float)size.y};
        sprite->Set_Param("MaskTextureSize", {&m_maskTextureSize, "float2", sizeof(_float2)});
    }

    sprite->ChangePass("UVAnimation");

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

    ImGui::SeparatorText("UV Animation");
    ImGui::DragFloat2("Speed", reinterpret_cast<_float*>(&m_vUVOffsetSpeed), 0.01f);

    auto sprite = Get_Component<CSprite2D>();

    ImGui::SeparatorText("Mask");
    if (ImGui::Checkbox("Enable", &m_useMask))
    {
        if (m_useMask)
        {
            sprite->Change_Texture(1, G_GlobalLevelKey, m_maskTextureKey);
            {
                auto maskTexture  = sprite->Get_Texture(1);
                auto maskSize     = maskTexture->Get_Size();
                m_maskTextureSize = {(float)maskSize.x, (float)maskSize.y};
                sprite->Set_Param("MaskTextureSize", {&m_maskTextureSize, "float2", sizeof(_float2)});
            }
            sprite->ChangePass("UVAnimation_Mask");
        }
        else
            sprite->ChangePass("UVAnimation");
    }

    if (ImGui::Checkbox("Debug Raw", &m_maskDebugRaw))
    {
        if (m_maskDebugRaw)
        {
            m_maskDebugApplied = false;
            m_useMask = true;

            sprite->Change_Texture(1, G_GlobalLevelKey, m_maskTextureKey);
            {
                auto maskTexture  = sprite->Get_Texture(1);
                auto maskSize     = maskTexture->Get_Size();
                m_maskTextureSize = {(float)maskSize.x, (float)maskSize.y};
                sprite->Set_Param("MaskTextureSize", {&m_maskTextureSize, "float2", sizeof(_float2)});
            }

            sprite->ChangePass("MaskDebugRaw");
        }
        else
        {
            if (m_useMask) sprite->ChangePass("UVAnimation_Mask");
            else           sprite->ChangePass("UVAnimation");
        }
    }

    if (ImGui::Checkbox("Debug Applied", &m_maskDebugApplied))
    {
        if (m_maskDebugApplied)
        {
            m_maskDebugRaw = false;
            m_useMask = true;

            sprite->Change_Texture(1, G_GlobalLevelKey, m_maskTextureKey);
            {
                auto maskTexture  = sprite->Get_Texture(1);
                auto maskSize     = maskTexture->Get_Size();
                m_maskTextureSize = {(float)maskSize.x, (float)maskSize.y};
                sprite->Set_Param("MaskTextureSize", {&m_maskTextureSize, "float2", sizeof(_float2)});
            }

            sprite->ChangePass("MaskDebugApplied");
        }
        else
        {
            if (m_useMask) sprite->ChangePass("UVAnimation_Mask");
            else           sprite->ChangePass("UVAnimation");
        }
    }

    ImGui::BeginDisabled(!m_useMask || m_maskDebugRaw || m_maskDebugApplied);

    if (ImGui::Button("Select Mask"))
    {
        string filePath = Helper::OpenFile({{"PNG Files", "*.png"}}, "png");
        if (!filePath.empty())
        {
            string fileName = Helper::GetFileNameWithExtension(filePath);
            CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath(fileName, filePath);
            m_maskTextureKey = fileName;

            sprite->Change_Texture(1, G_GlobalLevelKey, m_maskTextureKey);
            {
                auto maskTexture = sprite->Get_Texture(1);
                auto maskSize = maskTexture->Get_Size();
                m_maskTextureSize = {(float)maskSize.x, (float)maskSize.y};
                sprite->Set_Param("MaskTextureSize", {&m_maskTextureSize, "float2", sizeof(_float2)});
            }
        }
    }

    ImGui::DragFloat("Threshold", &m_maskThreshold, 0.01f, 0.f,  1.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
    ImGui::DragFloat("Softness",  &m_maskSoftness, 0.005f, 0.f, 0.2f, "%.3f", ImGuiSliderFlags_AlwaysClamp);

    sprite->Set_Param("MaskThreshold", {&m_maskThreshold, "float", sizeof(_float)});
    sprite->Set_Param("MaskSoftness",  {&m_maskSoftness,  "float", sizeof(_float)});

    ImGui::EndDisabled();
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