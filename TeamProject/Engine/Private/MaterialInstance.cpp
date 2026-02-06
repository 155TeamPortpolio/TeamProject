#include "Engine_Defines.h"
#include "MaterialInstance.h"
#include "MaterialData.h"
#include "Shader.h"
#include "GUIUtil.h"
#include "Texture.h"

_uint CMaterialInstance::Next_ID = 0;

CMaterialInstance::CMaterialInstance(CMaterialData* pData, ID3D11Device* pDevice)
	: m_pMaterialData{ pData }, m_pDevice{ pDevice }, m_MaterialInstance_ID{ ++Next_ID }
{
	Safe_AddRef(m_pMaterialData);
	Safe_AddRef(m_pDevice);
}

CMaterialInstance::CMaterialInstance(const CMaterialInstance& rhs)
	:m_pDevice{ rhs.m_pDevice }
	, m_pMaterialData{ rhs.m_pMaterialData }
	, m_TextureIndexs{ rhs.m_TextureIndexs }
	, overrides_Constant{ rhs.overrides_Constant }
	, m_MaterialInstance_ID{ ++Next_ID }

{
	Safe_AddRef(m_pMaterialData);
	Safe_AddRef(m_pDevice);
}

void CMaterialInstance::ApplyData(ID3D11DeviceContext* pContext)
{
	/*상수 버퍼*/
	CShader* pMaterialShader = m_pMaterialData->Get_Shader();
	for (size_t i = 0; i < MAX_TEXTURE_TYPE_VALUE; i++)
	{
		string constant = m_pMaterialData->ConvertToConstant(static_cast<TEXTURE_TYPE>(i));
		pMaterialShader->Bind_Value(constant, { nullptr,"Texture2D",0 });
	}

	m_pMaterialData->ApplyData(pContext, m_TextureIndexs);
	
	for (auto& Slot : m_DynamicSlots) {
		SHADER_PARAM tmpParam = Slot.second;
		pMaterialShader->Bind_Value(Slot.first, tmpParam);
	}

	pMaterialShader->Apply(Get_PassConstant(), pContext);
}

void CMaterialInstance::ClearDynamicSlotsBound(CShader* materialShader)
{
	for (const auto& slotPair : m_DynamicSlots)
	{
		const string& slotName = slotPair.first;
		SHADER_PARAM prevValue = slotPair.second;
		prevValue.pData = nullptr;
		materialShader->Bind_Value(slotName, prevValue);
	}

	m_DynamicSlots.clear();
}

const string& CMaterialInstance::Get_PassConstant()
{
	if (override_Pass.empty())
		return m_pMaterialData->Get_PassConstant();
	else
		return override_Pass;
}

CShader* CMaterialInstance::Get_Shader()
{
	return m_pMaterialData->Get_Shader();
}

_uint CMaterialInstance::Get_ShaderID()
{
	return m_pMaterialData->Get_ShaderID();
}

_uint CMaterialInstance::Get_MaterialDataID()
{
	return m_pMaterialData->Get_MaterialDataID();
}

const string& CMaterialInstance::Get_MaterialName()
{
	return m_pMaterialData->Get_MaterialName();
}

HRESULT CMaterialInstance::Set_Param(const string& ConstantName, const SHADER_PARAM& parameter)
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

SHADER_PARAM* CMaterialInstance::Get_Param(const string& ConstantName)
{
	auto iter = m_DynamicSlots.find(ConstantName);

	if (iter != m_DynamicSlots.end()) {
		return &(iter->second);
	}

	return nullptr;
}

HRESULT CMaterialInstance::Override_Constant(const MaterialConstants& materialConstant)
{
	overrides_Constant = materialConstant;
	return S_OK;
}

HRESULT CMaterialInstance::Reset_Constant()
{
	overrides_Constant = m_pMaterialData->Get_DefaultMaterialConstant();
	return S_OK;
}

void CMaterialInstance::Reset_DynamicSlot()
{
	//ClearDynamicSlotsBound(m_pMaterialData->Get_Shader());
	CShader* pShader = m_pMaterialData->Get_Shader();
	pShader->ResetToDefaults();
}
void CMaterialInstance::Reset_Textures()
{
	CShader* pShader = m_pMaterialData->Get_Shader();
	for (size_t i = 0; i < MAX_TEXTURE_TYPE_VALUE; i++)
	{
		string constant = m_pMaterialData->ConvertToConstant(static_cast<TEXTURE_TYPE>(i));
		SHADER_PARAM param = {};
		param.pData = nullptr;
		param.typeName = "Texture2D";
	
		pShader->Bind_Value(constant, param);
	}
}

void CMaterialInstance::ChangeTexture(TEXTURE_TYPE type, _uint index)
{
	m_TextureIndexs[static_cast<_uint>(type)]= index;
}

HRESULT CMaterialInstance::Reset_Pass()
{
	override_Pass = m_pMaterialData->Get_PassConstant();
	return S_OK;
}

_bool CMaterialInstance::isValid()
{
	return m_pMaterialData->Has_Texture(TEXTURE_TYPE::DIFFUSE);
}

void CMaterialInstance::SetBlendIf_AlphaDiffuse(AlphaCheckLevel level, const string &pass)
{
	_uint index= m_TextureIndexs[ENUM(TEXTURE_TYPE::DIFFUSE)];
	_bool isAlpha = m_pMaterialData->Has_NonOpaque(TEXTURE_TYPE::DIFFUSE, index, level);
	m_IsBlended= isAlpha;
	if (isAlpha)
		override_Pass = pass;
}

_bool CMaterialInstance::GetMaterialTextureKey(TEXTURE_TYPE type, _uint index, string& outKey)
{
    CTexture* pTexture = { nullptr };
    _bool indexIn = m_pMaterialData->Get_Texture(type, index, pTexture);

    if(!indexIn || nullptr == pTexture)
        return false;

    outKey = filesystem::path(pTexture->Get_Key()).filename().string();
    return true;
}

CTexture* CMaterialInstance::GetBindedTexture(TEXTURE_TYPE type)
{
    _uint nowIndex = m_TextureIndexs[ENUM(type)];

    CTexture* pTexture = { nullptr };
    _bool indexIn = m_pMaterialData->Get_Texture(type, nowIndex, pTexture);

    if (!indexIn || nullptr == pTexture)
        return nullptr;

    return pTexture;
}

_uint CMaterialInstance::Get_BindedIndex(TEXTURE_TYPE type)
{
    return  m_TextureIndexs[ENUM(type)];
}

_uint CMaterialInstance::Get_BindedCount(TEXTURE_TYPE type)
{
    return m_pMaterialData->Get_TextureCount(type);
}

void CMaterialInstance::Render_GUI()
{
    ImGui::PushID(this);

    CShader* shaderPtr = m_pMaterialData->Get_Shader();
    auto passList = shaderPtr->Get_PassList();
    const auto findPassIndex = [&](const string& passName) -> int
        {
            for (int passIndex = 0; passIndex < (int)passList.size(); ++passIndex)
            {
                if (passList[passIndex] == passName)
                    return passIndex;
            }
            return -1;
        };
    const auto ensureValidPass = [&]()
        {
            if (passList.empty())
            {
                override_Pass.clear();
                return;
            }

            if (override_Pass.empty())
                return;

            if (findPassIndex(override_Pass) < 0)
                override_Pass = passList[0];
        };

    const auto drawPassCombo = [&]()
        {
            if (passList.empty())
            {
                ImGui::TextDisabled("(No Pass)");
                return;
            }

            int currentIndex = findPassIndex(override_Pass.empty()? m_pMaterialData->Get_PassConstant():override_Pass);
            if (currentIndex < 0) currentIndex = 0;

            const char* previewText = passList[currentIndex].c_str();

            if (ImGui::BeginCombo("##shaderPass", previewText))
            {
                for (int passIndex = 0; passIndex < (int)passList.size(); ++passIndex)
                {
                    const bool isSelected = (passIndex == currentIndex);
                    if (ImGui::Selectable(passList[passIndex].c_str(), isSelected))
                    {
                        override_Pass = passList[passIndex];
                        currentIndex = passIndex;
                    }
                    if (isSelected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        };

    const auto pushTypeBadgeColor = [&](const string& typeName)
        {
            ImVec4 badgeColor = ImVec4(0.75f, 0.75f, 0.75f, 1.0f);
            if (typeName == "Texture2D") badgeColor = ImVec4(0.55f, 0.85f, 1.0f, 1.0f);
            else if (typeName == "float") badgeColor = ImVec4(0.80f, 1.0f, 0.65f, 1.0f);
            else if (typeName.find("float") != string::npos) badgeColor = ImVec4(0.80f, 1.0f, 0.65f, 1.0f);
            else if (typeName == "int" || typeName == "bool") badgeColor = ImVec4(1.0f, 0.85f, 0.55f, 1.0f);

            ImGui::PushStyleColor(ImGuiCol_Text, badgeColor);
        };

    const auto popTypeBadgeColor = [&]()
        {
            ImGui::PopStyleColor();
        };

    const auto drawBoundDot = [&](bool isBound)
        {
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2 cursorPos = ImGui::GetCursorScreenPos();
            float radius = 4.0f;

            ImU32 dotColor = isBound ? IM_COL32(110, 255, 110, 255) : IM_COL32(255, 110, 110, 255);
            drawList->AddCircleFilled(ImVec2(cursorPos.x + radius, cursorPos.y + radius + 2.0f), radius, dotColor);
            ImGui::Dummy(ImVec2(radius * 2.0f + 2.0f, radius * 2.0f));
        };

    const auto editParamValue = [&](const string& slotName, SHADER_PARAM& param)
        {
            if (param.typeName == "float")
            {
                float* valuePtr = reinterpret_cast<float*>(param.pData);
                if (!valuePtr) { ImGui::TextDisabled("(null)"); return; }
                ImGui::DragFloat("##v", valuePtr, 0.01f);
                return;
            }

            if (param.typeName == "float3")
            {
                float* valuePtr = reinterpret_cast<float*>(param.pData);
                if (!valuePtr) { ImGui::TextDisabled("(null)"); return; }
                ImGui::DragFloat3("##v", valuePtr, 0.01f);
                return;
            }

            if (param.typeName == "bool")
            {
                bool* valuePtr = reinterpret_cast<bool*>(param.pData);
                if (!valuePtr) { ImGui::TextDisabled("(null)"); return; }
                ImGui::Checkbox("##v", valuePtr);
                return;
            }

            if (param.typeName == "Texture2D")
            {
                void* textureHandle = reinterpret_cast<void*>(param.pData);
                if (!textureHandle) { ImGui::TextDisabled("(none)"); return; }
                ImGui::TextUnformatted("[Texture Bound]");
                // ImGui::Image((ImTextureID)textureHandle, ImVec2(64, 64));
                return;
            }

            ImGui::TextDisabled("No editor for type: %s", param.typeName.c_str());
        };

    ensureValidPass();
    drawPassCombo();

    if (m_pMaterialData)
        m_pMaterialData->Render_GUI(m_TextureIndexs);

    auto& slotsForPass = m_DynamicSlots;

    if (ImGui::CollapsingHeader("Dynamic Slots", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 4));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 6));

        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Slots");
        ImGui::SameLine();

        if (ImGui::SmallButton("Clear All"))
        {
          slotsForPass.clear();
        }

        ImGui::Separator();

        ImGuiTableFlags tableFlags =
            ImGuiTableFlags_BordersInnerV |
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_Resizable |
            ImGuiTableFlags_SizingStretchProp |
            ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_NoBordersInBodyUntilResize;

        const float tableHeight = 240.0f;

        if (ImGui::BeginTable("##DynamicSlotTable", 4, tableFlags, ImVec2(0, tableHeight)))
        {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 2.2f);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 110.0f);
            ImGui::TableSetupColumn("Bound", ImGuiTableColumnFlags_WidthFixed, 60.0f);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch, 2.6f);
            ImGui::TableHeadersRow();

            string removeKey;
            bool requestRemove = false;

            for (auto& nameAndParam : slotsForPass)
            {
                const string& slotName = nameAndParam.first;
                SHADER_PARAM& param = nameAndParam.second;

                const bool isBound = (param.pData != nullptr);

                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::PushID(slotName.c_str());

                ImGuiTreeNodeFlags nodeFlags =
                    ImGuiTreeNodeFlags_Leaf |
                    ImGuiTreeNodeFlags_NoTreePushOnOpen |
                    ImGuiTreeNodeFlags_SpanFullWidth;

                ImGui::TreeNodeEx("##slotnode", nodeFlags, "%s", slotName.c_str());

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

                ImGui::TableSetColumnIndex(1);
                pushTypeBadgeColor(param.typeName);
                ImGui::TextUnformatted(param.typeName.c_str());
                popTypeBadgeColor();

                // Bound
                ImGui::TableSetColumnIndex(2);
                ImGui::PushStyleColor(ImGuiCol_Text,
                    isBound ? ImVec4(0.55f, 1.0f, 0.55f, 1.0f)
                    : ImVec4(1.0f, 0.55f, 0.55f, 1.0f));
                drawBoundDot(isBound);
                ImGui::SameLine();
                ImGui::TextUnformatted(isBound ? "Yes" : "No");
                ImGui::PopStyleColor();

                // Value
                ImGui::TableSetColumnIndex(3);
                ImGui::PushItemWidth(-FLT_MIN);
                editParamValue(slotName, param);
                ImGui::PopItemWidth();
            }

            ImGui::EndTable();

            if (requestRemove && !removeKey.empty())
                slotsForPass.erase(removeKey);
        }

        ImGui::Spacing();
        ImGui::TextDisabled("Tip: Right-click a slot for actions (Unbind/Remove).");

        ImGui::PopStyleVar(2);
    }

    ImGui::PopID();
}

void CMaterialInstance::TypeCheck(const std::string& slotName, SHADER_PARAM& param)
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

        const ImVec2 thumbSize(40, 40);
        ImGui::Image((ImTextureID)srvPtr, thumbSize);

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

CMaterialInstance* CMaterialInstance::Make_Handle(CMaterialData* pData, ID3D11Device* pDevice)
{
	CMaterialInstance* hMaterial = new CMaterialInstance(pData, pDevice);

	hMaterial->overrides_Constant = pData->Get_DefaultMaterialConstant();
	hMaterial->m_TextureIndexs.resize(MAX_TEXTURE_TYPE_VALUE, 0);

	return hMaterial;
}

CMaterialInstance* CMaterialInstance::Create_Handle(const string& materialKey, const string& DefualtpassConstant, ID3D11Device* pDevice)
{
	CMaterialData* pData = CMaterialData::Create(materialKey, DefualtpassConstant);
	CMaterialInstance* hMaterial = new CMaterialInstance(pData, pDevice);

	hMaterial->overrides_Constant = pData->Get_DefaultMaterialConstant();
	hMaterial->m_TextureIndexs.resize(MAX_TEXTURE_TYPE_VALUE, 0);
	hMaterial->override_Pass = DefualtpassConstant;

	/*직접 생성해주었으니, 안에 넣고 나면 addRef되고,
	그거 레퍼런스 카운트 하나 다운 시켜주어야 함*/
	Safe_Release(pData);

	return hMaterial;
}

CMaterialInstance* CMaterialInstance::Clone()
{
	CMaterialInstance* hMaterial = new CMaterialInstance(*this);

	return hMaterial;
}

void CMaterialInstance::Free()
{
	Safe_Release(m_pMaterialData);
	Safe_Release(m_pDevice);
}