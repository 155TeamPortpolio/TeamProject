#include "Engine_Defines.h"
#include "GameInstance.h"
#include "Sprite2D.h"
#include "Shader.h"
#include "Texture.h"
#include "IResourceService.h"
#include "VI_Point.h"
#include "UI_Object.h"
#include "GUIUtil.h"

CSprite2D::CSprite2D(const CSprite2D& rhs)
	:CComponent(rhs),
	m_pShader(rhs.m_pShader),
	m_pPoint(rhs.m_pPoint),
	m_pTextures(rhs.m_pTextures),
	m_TextKey(rhs.m_TextKey)
{
	Safe_AddRef(m_pShader);
	Safe_AddRef(m_pPoint);
	for (auto tex : m_pTextures)
		Safe_AddRef(tex);
}

HRESULT CSprite2D::Initialize_Prototype()
{
	m_pPoint = CVI_Point::Create(CGameInstance::GetInstance()->Get_Device(), "Sprite2D");

	return S_OK;
}

HRESULT CSprite2D::Initialize(COMPONENT_DESC* pArg)
{
	if (m_pPoint == nullptr)
		return E_FAIL;

	return S_OK;
}

void CSprite2D::Apply_Shader(ID3D11DeviceContext* pContext)
{
	if (m_pShader == nullptr) return;

    ID3D11ShaderResourceView* nullSRV[1]{};

	for (UINT slot = 0; slot < MAX_TEXTURE_TYPE_VALUE; ++slot)
		pContext->PSSetShaderResources(slot, 1, nullSRV);

	if (!m_pTextures.empty() && m_pTextures[m_iDrawIndex] != nullptr)
    {
		SHADER_PARAM param = {};
		param.typeName     = "Texture2D";
		param.iSize        = 0;
		param.pData        = m_pTextures[m_iDrawIndex]->Get_SRV();

		m_pShader->Bind_Value("SpriteTexture", param);
	}

    if (m_pTextures.size() > 1 && m_pTextures[1] != nullptr)
    {
        SHADER_PARAM param = {};
        param.typeName     = "Texture2D";
        param.iSize        = 0;
        param.pData        = m_pTextures[1]->Get_SRV();

        m_pShader->Bind_Value("ColorTexture", param);
    }

	for (auto& Slot : m_DynamicSlots) 
		m_pShader->Bind_Value(Slot.first, Slot.second);

	m_pShader->Apply(m_PassConstant, pContext);
}

void CSprite2D::Draw_Sprite(ID3D11DeviceContext* pContext)
{
	if (m_pPoint)
	{
		m_pPoint->Bind_Buffer(pContext);
		m_pPoint->Render(pContext);
	}
}

HRESULT CSprite2D::ChangeSprite(_uint Index)
{
	if (Index >= m_pTextures.size()) 
		return E_FAIL;
	

	if(m_pTextures[Index] == nullptr)
		return E_FAIL;

	m_iDrawIndex = Index;
	return S_OK;
}

CVIBuffer* CSprite2D::Get_Buffer()
{
	return m_pPoint;
}

HRESULT CSprite2D::Add_Texture(const string& levelKey, const string& texKey)
{
	CTexture* pTexture = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_Texture(levelKey, texKey, true);

	if (!pTexture)
		return E_FAIL;
	m_pTextures.push_back(pTexture);

	Safe_AddRef(pTexture);
	return S_OK;
}

HRESULT CSprite2D::Change_Texture(_uint idx, const string& levelKey, const string& texKey)
{
	if (idx >= m_pTextures.size())
		Add_Texture(levelKey, texKey);
	else
	{
		if (m_pTextures[idx])
			Safe_Release(m_pTextures[idx]);

		auto tex = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_Texture(levelKey, texKey, true);
		if (!tex)
			return E_FAIL;

		m_pTextures[idx] = tex;
		Safe_AddRef(tex);
	}
	return S_OK;
}

HRESULT CSprite2D::Link_Shader(const string& levelKey, const string& shaderKey)
{
	Safe_Release(m_pShader);
	m_pShader = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_Shader(levelKey, shaderKey);

	if (!m_pShader)
		return E_FAIL;

	Safe_AddRef(m_pShader);
	return S_OK;
}

HRESULT CSprite2D::ChangePass(const string& passConstant)
{
	if (passConstant.empty())
		return S_OK;

	m_PassConstant = passConstant;
	return S_OK;
}

HRESULT CSprite2D::Set_TextKey(const string& textKey)
{
	m_TextKey = textKey;
	return S_OK;
}

HRESULT CSprite2D::Set_Param(const string& ConstantName, const SHADER_PARAM& parameter)
{
	auto it = m_DynamicSlots.find(ConstantName);
	if (it != m_DynamicSlots.end())
	{
		it->second = parameter;    // 기존 값 덮어쓰기
		return S_OK;
	}
	SHADER_PARAM newparameter = parameter;
	m_DynamicSlots.emplace(ConstantName, newparameter);
	return S_OK;
}

SHADER_PARAM* CSprite2D::Get_Param(const string& ConstantName)
{
	auto iter = m_DynamicSlots.find(ConstantName);

	if (iter != m_DynamicSlots.end()) {
		return &(iter->second);
	}

	return nullptr;
}

bool CSprite2D::IsValid()
{
	if (m_pPoint == nullptr || m_pShader == nullptr)
		return false;

	if (!m_pTextures.empty())
		return true;

	auto it = m_DynamicSlots.find("SpriteTexture");
	if (it != m_DynamicSlots.end())
		return true;

	return false;
}

void CSprite2D::Render_GUI()
{
	if (!m_pTextures.empty()) 
	{
		ImGui::Image((ImTextureID)m_pTextures.front()->Get_SRV(),
			ImVec2(1280 / 5, 720 / 5));
	}

    if (ImGui::CollapsingHeader("Dynamic Slots"))
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 4));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 6));

        // 상단 툴바
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Slots");
        ImGui::SameLine();

        if (ImGui::SmallButton("Reset Defaults"))
        {
            m_pShader->ResetToDefaults();
        }

        ImGui::SameLine();
        if (ImGui::SmallButton("Clear All"))
        {
            m_DynamicSlots.clear();
        }

        ImGui::Separator();

        ImGuiTableFlags flags =
            ImGuiTableFlags_BordersInnerV |
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_Resizable |
            ImGuiTableFlags_SizingStretchProp |
            ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_NoBordersInBodyUntilResize;

        const float tableHeight = 240.0f;

        if (ImGui::BeginTable("##DynamicSlotTable", 4, flags, ImVec2(0, tableHeight)))
        {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 2.2f);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 110.0f);
            ImGui::TableSetupColumn("Bound", ImGuiTableColumnFlags_WidthFixed, 60.0f);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch, 2.6f);
            ImGui::TableHeadersRow();

            std::string removeKey;
            bool requestRemove = false;

            for (auto& pair : m_DynamicSlots)
            {
                const std::string& slotName = pair.first;
                SHADER_PARAM& param = pair.second;

                const bool isBound = (param.pData != nullptr);

                ImGui::TableNextRow();

                // Name (TreeNode 느낌)
                ImGui::TableSetColumnIndex(0);
                ImGui::PushID(slotName.c_str());

                ImGuiTreeNodeFlags nodeFlags =
                    ImGuiTreeNodeFlags_Leaf |
                    ImGuiTreeNodeFlags_NoTreePushOnOpen |
                    ImGuiTreeNodeFlags_SpanFullWidth;

                ImGui::TreeNodeEx("##slotnode", nodeFlags, "%s", slotName.c_str());

                // 우클릭 메뉴
                if (ImGui::BeginPopupContextItem("##slotctx"))
                {
                    if (ImGui::MenuItem("Unbind"))
                        param.pData = nullptr;

                    if (ImGui::MenuItem("Remove"))
                    {
                        removeKey = slotName;
                        requestRemove = true;
                    }

                    ImGui::EndPopup();
                }

                ImGui::PopID();

                // Type (배지)
                ImGui::TableSetColumnIndex(1);
                GuiUtil::PushTypeBadgeColor(param.typeName);
                ImGui::TextUnformatted(param.typeName.c_str());
                GuiUtil::PopTypeBadgeColor();

                // Bound (점 + 텍스트)
                ImGui::TableSetColumnIndex(2);
                ImGui::PushStyleColor(ImGuiCol_Text, isBound ? ImVec4(0.55f, 1.0f, 0.55f, 1.0f)
                    : ImVec4(1.0f, 0.55f, 0.55f, 1.0f));
                GuiUtil::DrawBoundDot(isBound);
                ImGui::SameLine();
                ImGui::TextUnformatted(isBound ? "Yes" : "No");
                ImGui::PopStyleColor();

                // Value/Editor
                ImGui::TableSetColumnIndex(3);

                // 한 줄에 딱 맞게 아이템 폭 조절
                ImGui::PushItemWidth(-FLT_MIN);
                TypeCheck(slotName, param);
                ImGui::PopItemWidth();
            }

            ImGui::EndTable();

            if (requestRemove && !removeKey.empty())
                m_DynamicSlots.erase(removeKey);
        }

        ImGui::Spacing();
        ImGui::TextDisabled("Tip: Right-click a slot for actions (Unbind/Remove).");

        ImGui::PopStyleVar(2);
    }
}

void CSprite2D::TypeCheck(const std::string& slotName, SHADER_PARAM& param)
{
    const std::string& typeName = param.typeName;

    if (typeName == "Texture2D")
    {
        ID3D11ShaderResourceView* srvPtr =
            reinterpret_cast<ID3D11ShaderResourceView*>(param.pData);

        if (!srvPtr)
        {
            ImGui::TextDisabled("(null SRV)");
            return;
        }

        // 미니 썸네일
        const ImVec2 thumbSize(40, 40);
        ImGui::Image((ImTextureID)srvPtr, thumbSize);

        // 호버 시 확대 툴팁
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(slotName.c_str());
            ImGui::Separator();
            ImGui::Image((ImTextureID)srvPtr, ImVec2(256, 256));
            ImGui::EndTooltip();
        }

        ImGui::SameLine();
        ImGui::TextDisabled(" (hover to preview)");
        return;
    }

    if (typeName == "bool")
    {
        bool* boolPtr = reinterpret_cast<bool*>(param.pData);
        bool value = boolPtr ? *boolPtr : false;

        if (ImGui::Checkbox(("##val_" + slotName).c_str(), &value))
        {
            if (boolPtr) *boolPtr = value;
        }
        return;
    }
    if (typeName == "bool4")
    {
        bool* boolPtr = reinterpret_cast<bool*>(param.pData);
        bool tmp[4] = { false,false,false,false };
        if (boolPtr) { tmp[0] = boolPtr[0]; tmp[1] = boolPtr[1]; tmp[2] = boolPtr[2]; tmp[3] = boolPtr[3]; }

        bool changed = false;
        changed |= ImGui::Checkbox(("X##b4_" + slotName).c_str(), &tmp[0]); ImGui::SameLine();
        changed |= ImGui::Checkbox(("Y##b4_" + slotName).c_str(), &tmp[1]); ImGui::SameLine();
        changed |= ImGui::Checkbox(("Z##b4_" + slotName).c_str(), &tmp[2]); ImGui::SameLine();
        changed |= ImGui::Checkbox(("W##b4_" + slotName).c_str(), &tmp[3]);

        if (changed && boolPtr)
        {
            boolPtr[0] = tmp[0]; boolPtr[1] = tmp[1]; boolPtr[2] = tmp[2]; boolPtr[3] = tmp[3];
        }
        return;
    }

    if (typeName == "float")
    {
        float* valuePtr = reinterpret_cast<float*>(param.pData);
        float value = valuePtr ? *valuePtr : 0.0f;

        if (ImGui::DragFloat(("##val_" + slotName).c_str(), &value, 0.01f))
        {
            if (valuePtr) *valuePtr = value;
        }
        return;
    }
    if (typeName == "float2")
    {
        float* vecPtr = reinterpret_cast<float*>(param.pData);
        float tmp[2] = { 0,0 };
        if (vecPtr) { tmp[0] = vecPtr[0]; tmp[1] = vecPtr[1]; }

        if (ImGui::DragFloat2(("##val_" + slotName).c_str(), tmp, 0.01f))
        {
            if (vecPtr) { vecPtr[0] = tmp[0]; vecPtr[1] = tmp[1]; }
        }
        return;
    }
    if (typeName == "float3")
    {
        float* vecPtr = reinterpret_cast<float*>(param.pData);
        float tmp[3] = { 0,0,0 };
        if (vecPtr) { tmp[0] = vecPtr[0]; tmp[1] = vecPtr[1]; tmp[2] = vecPtr[2]; }

        if (ImGui::DragFloat3(("##val_" + slotName).c_str(), tmp, 0.01f))
        {
            if (vecPtr) { vecPtr[0] = tmp[0]; vecPtr[1] = tmp[1]; vecPtr[2] = tmp[2]; }
        }
        return;
    }
    if (typeName == "float4")
    {
        float* vecPtr = reinterpret_cast<float*>(param.pData);
        float tmp[4] = { 0,0,0,0 };
        if (vecPtr) { tmp[0] = vecPtr[0]; tmp[1] = vecPtr[1]; tmp[2] = vecPtr[2]; tmp[3] = vecPtr[3]; }

        if (ImGui::DragFloat4(("##val_" + slotName).c_str(), tmp, 0.01f))
        {
            if (vecPtr) { vecPtr[0] = tmp[0]; vecPtr[1] = tmp[1]; vecPtr[2] = tmp[2]; vecPtr[3] = tmp[3]; }
        }
        return;
    }

    if (typeName == "int")
    {
        int* valuePtr = reinterpret_cast<int*>(param.pData);
        int value = valuePtr ? *valuePtr : 0;

        if (ImGui::DragInt(("##val_" + slotName).c_str(), &value, 1))
        {
            if (valuePtr) *valuePtr = value;
        }
        return;
    }
    if (typeName == "int2")
    {
        int* vecPtr = reinterpret_cast<int*>(param.pData);
        int tmp[2] = { 0,0 };
        if (vecPtr) { tmp[0] = vecPtr[0]; tmp[1] = vecPtr[1]; }

        if (ImGui::DragInt2(("##val_" + slotName).c_str(), tmp, 1))
        {
            if (vecPtr) { vecPtr[0] = tmp[0]; vecPtr[1] = tmp[1]; }
        }
        return;
    }
    if (typeName == "int3")
    {
        int* vecPtr = reinterpret_cast<int*>(param.pData);
        int tmp[3] = { 0,0,0 };
        if (vecPtr) { tmp[0] = vecPtr[0]; tmp[1] = vecPtr[1]; tmp[2] = vecPtr[2]; }

        if (ImGui::DragInt3(("##val_" + slotName).c_str(), tmp, 1))
        {
            if (vecPtr) { vecPtr[0] = tmp[0]; vecPtr[1] = tmp[1]; vecPtr[2] = tmp[2]; }
        }
        return;
    }
    if (typeName == "int4")
    {
        int* vecPtr = reinterpret_cast<int*>(param.pData);
        int tmp[4] = { 0,0,0,0 };
        if (vecPtr) { tmp[0] = vecPtr[0]; tmp[1] = vecPtr[1]; tmp[2] = vecPtr[2]; tmp[3] = vecPtr[3]; }

        if (ImGui::DragInt4(("##val_" + slotName).c_str(), tmp, 1))
        {
            if (vecPtr) { vecPtr[0] = tmp[0]; vecPtr[1] = tmp[1]; vecPtr[2] = tmp[2]; vecPtr[3] = tmp[3]; }
        }
        return;
    }

    if (typeName == "uint")
    {
        unsigned int* valuePtr = reinterpret_cast<unsigned int*>(param.pData);
        unsigned int value = valuePtr ? *valuePtr : 0u;

        if (ImGui::DragScalar(("##val_" + slotName).c_str(), ImGuiDataType_U32, &value, 1.0f))
        {
            if (valuePtr) *valuePtr = value;
        }
        return;
    }
    if (typeName == "uint2")
    {
        unsigned int* vecPtr = reinterpret_cast<unsigned int*>(param.pData);
        unsigned int tmp[2] = { 0u,0u };
        if (vecPtr) { tmp[0] = vecPtr[0]; tmp[1] = vecPtr[1]; }

        if (ImGui::DragScalarN(("##val_" + slotName).c_str(), ImGuiDataType_U32, tmp, 2, 1.0f))
        {
            if (vecPtr) { vecPtr[0] = tmp[0]; vecPtr[1] = tmp[1]; }
        }
        return;
    }
    if (typeName == "uint3")
    {
        unsigned int* vecPtr = reinterpret_cast<unsigned int*>(param.pData);
        unsigned int tmp[3] = { 0u,0u,0u };
        if (vecPtr) { tmp[0] = vecPtr[0]; tmp[1] = vecPtr[1]; tmp[2] = vecPtr[2]; }

        if (ImGui::DragScalarN(("##val_" + slotName).c_str(), ImGuiDataType_U32, tmp, 3, 1.0f))
        {
            if (vecPtr) { vecPtr[0] = tmp[0]; vecPtr[1] = tmp[1]; vecPtr[2] = tmp[2]; }
        }
        return;
    }
    if (typeName == "uint4")
    {
        unsigned int* vecPtr = reinterpret_cast<unsigned int*>(param.pData);
        unsigned int tmp[4] = { 0u,0u,0u,0u };
        if (vecPtr) { tmp[0] = vecPtr[0]; tmp[1] = vecPtr[1]; tmp[2] = vecPtr[2]; tmp[3] = vecPtr[3]; }

        if (ImGui::DragScalarN(("##val_" + slotName).c_str(), ImGuiDataType_U32, tmp, 4, 1.0f))
        {
            if (vecPtr) { vecPtr[0] = tmp[0]; vecPtr[1] = tmp[1]; vecPtr[2] = tmp[2]; vecPtr[3] = tmp[3]; }
        }
        return;
    }

    ImGui::Text("Unknown_Type: %s, ptr: 0x%p", typeName.c_str(), param.pData);
}

_float CSprite2D::Get_AspectRatio()
{
	if (m_pTextures.empty() || m_pTextures.size() <= m_iDrawIndex || !m_pTextures[m_iDrawIndex])
		return 1.f;

	const _uint2 vSize = m_pTextures[m_iDrawIndex]->Get_Size();

	if (vSize.x == 0 || vSize.y == 0)
		return 1.f;

	return vSize.x / static_cast<_float>(vSize.y);
}

_uint2 CSprite2D::Get_TextureSize()
{
	if (m_pTextures.empty() || m_pTextures.size() <= m_iDrawIndex || !m_pTextures[m_iDrawIndex])
		return { 1, 1 };

	const _uint2 vSize = m_pTextures[m_iDrawIndex]->Get_Size();

	if (vSize.x == 0 || vSize.y == 0)
		return { 1, 1 };

	return vSize;
}

CSprite2D* CSprite2D::Create()
{
	CSprite2D* instance = new CSprite2D();
	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Sprite2D Comp Failed To Create : CSprite2D");
	}
	return instance;
}

void CSprite2D::Free()
{
	__super::Free();
	Safe_Release(m_pPoint);
	Safe_Release(m_pShader);

    for (auto& texture : m_pTextures) {
        if(texture->Get_SRV())
		    Safe_Release(texture);
    }
}