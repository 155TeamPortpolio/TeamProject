#include "pch.h"
#include "UIObject_Tool.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"
#include "Helper_Func.h"

CUIObject_Tool::CUIObject_Tool()
{
}

CUIObject_Tool::CUIObject_Tool(const CUIObject_Tool& rhs)
    : CUI_Object(rhs)
{
}

HRESULT CUIObject_Tool::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    // GUI Inspector 창에 띄움
    CGameInstance::GetInstance()->Get_GUISystem()->Get_Context()->pSelectedObject = this;

    return S_OK;
}

void CUIObject_Tool::Render_GUI()
{
    ImGui::SeparatorText(u8"속성");

    ImGui::Checkbox("Alive", &m_isAlive);

    Render_GUI_Animation();
}

void CUIObject_Tool::DestroyChild_FromParent()
{
    if (!m_pParent)
        return;

    auto pParentContainer = m_pParent->Get_Component<CObjectContainer>();
    if (!pParentContainer)
        return;

    pParentContainer->Destroy_Child(m_iChildIndex);
}

void CUIObject_Tool::ToJson(json& data)
{
    ToJson_Common(data);

    ToJson_Parent(data);
}

void CUIObject_Tool::FromJson(const json& data)
{
    FromJson_LinkParent(data);
}

void CUIObject_Tool::ToJson_Common(json& data)
{ 
    // 기본 정보
    data["levelTag"] = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey(); // 현재 레벨을 가져와야하나 아니면 객체에 저장된 레벨을 가져와야하나
    data["instanceKey"] = m_InstanceName;

    // Transform 정보
    data["transform"]["anchorOffset"]["x"] = m_vAnchorOffset.x;
    data["transform"]["anchorOffset"]["y"] = m_vAnchorOffset.y;
    data["transform"]["anchor"] = static_cast<int>(m_eAnchor);
    data["transform"]["size"]["x"] = m_vSize.x;
    data["transform"]["size"]["y"] = m_vSize.y;
    data["transform"]["scale"]["x"] = m_vScale.x;
    data["transform"]["scale"]["y"] = m_vScale.y;
    data["transform"]["pivot"]["x"] = m_vPivot.x;
    data["transform"]["pivot"]["y"] = m_vPivot.y;
    data["transform"]["rotation"] = m_fRadian;
}

void CUIObject_Tool::ToJson_Parent(json& data)
{
    if (!m_pParent || -1 == m_iChildIndex)
        return;

    const string& strCurrentLevel = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();
    auto& Objects = CGameInstance::GetInstance()->Get_UIMgr()->Get_LevelUI(strCurrentLevel);

    string strParentInstanceKey;
    for (auto& pObj : Objects)
    {
        if (pObj == m_pParent)
        {
            strParentInstanceKey = pObj->Get_InstanceName();
            break;
        }
    }

    if (strParentInstanceKey.empty())
        return;

    data["parent"]["instanceKey"] = strParentInstanceKey;
    data["parent"]["childIndex"] = m_iChildIndex;
}

void CUIObject_Tool::FromJson_LinkParent(const json& data)
{
    if (!data.contains("parent") || !data["parent"].size())
        return;

    vector<CUI_Object*> objects = CGameInstance::GetInstance()->Get_UIMgr()->Get_LevelUI(data["levelTag"]);

    for (auto& pObj : objects)
    {
        if (data["parent"]["instanceKey"] == pObj->Get_InstanceName())
        {
            CUIObject_Tool* pUIObj = dynamic_cast<CUIObject_Tool*>(pObj);
            if (!pUIObj)
                break;

            m_pParent = pUIObj;
            m_iChildIndex = data["parent"]["childIndex"];

            pObj->Get_Component<CObjectContainer>()->Add_Child(this);

            break;
        }
    }
}

void CUIObject_Tool::Add_Child(CUIObject_Tool* pChild)
{
    if (!pChild || pChild == this)
        return;

    auto pContainer = Get_Component<CObjectContainer>();
   if (!pContainer)
       return;

   if (pChild->m_pParent)
       pChild->m_pParent->Remove_Child(pChild);

   _int iIndex = pContainer->Add_Child(pChild);
   if (-1 == iIndex)
       return;

   pChild->m_pParent = this;
   pChild->m_iChildIndex = iIndex;
}

void CUIObject_Tool::Remove_Child(CUIObject_Tool* pChild)
{
    auto pContainer = Get_Component<CObjectContainer>();
    if (!pContainer)
        return;

    _int index = pContainer->Find_IndexByName(pChild->Get_InstanceName());
    if (index == -1)
        return;

    pContainer->Destroy_Child(index);
    pChild->m_pParent = nullptr;
    pChild->m_iChildIndex = -1;
}

void CUIObject_Tool::FromJson_RefreshCount(_uint& iCount)
{
    int index = {};
    for (char c : Get_InstanceName())
    {
        if (isdigit(c))
            index = index * 10 + (c - '0');
    }

    iCount = max(iCount, index + 1);
}

void CUIObject_Tool::Render_GUI_Layout()
{
    ImGui::SeparatorText("Layout");

    ImGui::Text("Anchor");
    struct AnchorPreset { const char* label; ANCHOR value; };
    static const AnchorPreset presets[9] = {
        {"##TL", (ANCHOR)((_uint)ANCHOR::Top | (_uint)ANCHOR::Left)},
        {"##TC", (ANCHOR)((_uint)ANCHOR::Top)},
        {"##TR", (ANCHOR)((_uint)ANCHOR::Top | (_uint)ANCHOR::Right)},

        {"##CL", (ANCHOR)((_uint)ANCHOR::Left)},
        {"##CC", (ANCHOR)((_uint)ANCHOR::Center)},
        {"##CR", (ANCHOR)((_uint)ANCHOR::Right)},

        {"##BL", (ANCHOR)((_uint)ANCHOR::Bottom | (_uint)ANCHOR::Left)},
        {"##BC", (ANCHOR)((_uint)ANCHOR::Bottom)},
        {"##BR", (ANCHOR)((_uint)ANCHOR::Bottom | (_uint)ANCHOR::Right)},
    };

    auto IsSelected = [&](ANCHOR a) { return (_uint)a == (_uint)m_eAnchor; };

    float cell = ImGui::GetFrameHeight(); // 정사각형 느낌
    ImVec2 btnSize(cell * 1.2f, cell * 1.2f);

    for (int i = 0; i < 9; ++i)
    {
        if (i % 3 != 0) ImGui::SameLine();

        bool selected = IsSelected(presets[i].value);
        if (selected) ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
        else          ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);

        if (selected) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));

        if (ImGui::Button(presets[i].label, btnSize))
            m_eAnchor = presets[i].value;

        if (selected) ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    ImGui::TextDisabled("Selected: %u", (_uint)m_eAnchor);

    ImGui::DragFloat2(u8"위치", reinterpret_cast<_float*>(&m_vAnchorOffset));

    ImGui::DragFloat2(u8"크기", reinterpret_cast<_float*>(&m_vSize), 1.f, 0.f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp);
}

void CUIObject_Tool::Render_GUI_Transform()
{ 
    ImGui::SeparatorText("Transform");

    ImGui::DragFloat2("Scale", reinterpret_cast<_float*>(&m_vScale), 0.01f, 0.f, 10.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);

    _float fDegree = XMConvertToDegrees(m_fRadian);
    if(ImGui::DragFloat("Angle", &fDegree, 1.f, -180.f, 180.f))
        m_fRadian = XMConvertToRadians(fDegree);

    _float2 tmpPivot = m_vPivot;
    if (ImGui::DragFloat2("Pivot", reinterpret_cast<_float*>(&tmpPivot), 0.01f, 0.f, 1.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
        Set_Pivot(tmpPivot);

    ImGui::TextDisabled("LeftTop : %.1f, %.1f", m_vLeftTop.x, m_vLeftTop.y);

    ImGui::TextDisabled("WinSize : %.1f x %.1f", m_WinSize.x, m_WinSize.y);
}

void CUIObject_Tool::Render_GUI_Animation()
{
    ImGui::SeparatorText(u8"애니메이션");

    // 클립 추가
    {
        ImGui::AlignTextToFramePadding();
        ImGui::Text(u8"클립 : ");

        ImGui::SameLine();
        static _int iCount = 0;
        if (ImGui::Button(u8"추가 +"))
            m_AnimClips.push_back(UI_ANIM_CLIP("clip" + to_string(iCount++)));

        ImGui::SameLine();
        ImGui::BeginDisabled(m_AnimClips.empty());
        if (ImGui::Button(u8"삭제 -"))
        {
            m_AnimClips.pop_back();
            iCount--;
        } 
        ImGui::EndDisabled();
    } 

    // 애니메이션 클립이 없으면 리턴
    if (m_AnimClips.empty())
        return;

    // 클립 선택
    static _int iClipIndex = -1;
    string strCombined;
    for (_int i = 0; i < m_AnimClips.size(); ++i)
        strCombined += m_AnimClips[i].strName + '\0';
    strCombined += '\0';
    
    ImGui::Combo(u8"클립", &iClipIndex, strCombined.c_str());
    
    // 클립 선택 없으면 리턴
    if (-1 == iClipIndex)
        return;

    static bool showPopup = false;

    if (ImGui::Button(u8"클립 편집"))
        showPopup = true;

    if (showPopup)
    {
        ImGui::SetNextWindowPos(ImVec2(880, 400), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(200, 300), ImGuiCond_Once);
        ImGui::Begin(m_AnimClips[iClipIndex].strName.c_str(), &showPopup);

        // 선택한 클립 편집
        ImGui::SeparatorText(u8"클립");
        char szBuffer[256] = {};
        strcpy_s(szBuffer, m_AnimClips[iClipIndex].strName.c_str());
        if (ImGui::InputText(u8"이름", szBuffer, sizeof(szBuffer)))
            m_AnimClips[iClipIndex].strName = szBuffer;

        ImGui::InputFloat(u8"길이", &m_AnimClips[iClipIndex].fDuration);
        ImGui::Checkbox(u8"루프", &m_AnimClips[iClipIndex].isLoop);
        ImGui::BeginDisabled(m_AnimClips[iClipIndex].keyframes.empty());
        if (ImGui::Button(m_isBlending ? u8"정지" : u8"재생"))
        {
            m_isBlending = !m_isBlending;
            if (m_isBlending)
                m_iCurrentClipIndex = iClipIndex;
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        if (m_isBlending)   ImGui::TextColored(ImVec4(0.2f, 1.f, 0.2f, 1.f), u8"● Playing");
        else                ImGui::TextColored(ImVec4(1.f, 0.3f, 0.3f, 1.f), u8"■ Stopped");

        // 키프레임 추가
        ImGui::SeparatorText(u8"키프레임");
        if (ImGui::Button(u8"추가 +"))
        {
            _float fTime = {};
            if (!m_AnimClips[iClipIndex].keyframes.empty())
                fTime = min(m_AnimClips[iClipIndex].keyframes.back().fTime + 0.5f, m_AnimClips[iClipIndex].fDuration);

            m_AnimClips[iClipIndex].keyframes.push_back(UI_KEYFRAME(fTime));
        }
        ImGui::SameLine();
        if (ImGui::Button(u8"삭제 -"))
        {
            if(!m_AnimClips[iClipIndex].keyframes.empty())
                m_AnimClips[iClipIndex].keyframes.pop_back();
        }

        // 키프레임 편집 
        ImGui::Separator();
        int idx = 0;
        for (auto& keyframe : m_AnimClips[iClipIndex].keyframes)
        {
            ImGui::PushID(idx);

            if (ImGui::TreeNodeEx(("Keyframe : " + to_string(keyframe.fTime)).c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_NoTreePushOnOpen))
            {
                ImGui::TreePush("##Content");

                ImGui::DragFloat(u8"시간", &keyframe.fTime, 0.1f, 0.f, m_AnimClips[iClipIndex].fDuration, "%.2f", ImGuiSliderFlags_AlwaysClamp);
                ImGui::DragFloat2(u8"스케일", reinterpret_cast<_float*>(&keyframe.vScale), 0.1f, 0.f, 100.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
                ImGui::DragFloat(u8"각도", &keyframe.fAngle, 1.f, -180.f, 180.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
                ImGui::DragFloat2(u8"위치", reinterpret_cast<_float*>(&keyframe.vPosition), 0.1f);
                ImGui::ColorEdit4(u8"컬러", reinterpret_cast<_float*>(&keyframe.vColor));

                Helper::DrawEnumCombo("id", keyframe.easeType, 100.f);

                ImGui::TreePop();
            }

            ImGui::PopID();
            ++idx;
        }

        ImGui::End();
    } 
}

void CUIObject_Tool::Play_Animation(_float dt)
{
    if (m_iCurrentClipIndex < 0 || m_iCurrentClipIndex >= m_AnimClips.size())
        return;

    if (m_isBlending)
    {
        const UI_ANIM_CLIP& clip = m_AnimClips[m_iCurrentClipIndex];

        if (clip.keyframes.empty())
        {
            m_isBlending = false;
            return;
        } 

        m_fBlendTime += dt;
        _float fRatio = {};

        if (clip.fDuration > 0.f)
            fRatio = m_fBlendTime / clip.fDuration;

        fRatio = clamp(fRatio, 0.f, 1.f);

        // 현재 시간에 해당하는 키프레임 찾기
        _int iCurrentKeyIdx = -1;
        _int iNextKeyIdx = -1;

        for (int i = 0; i < clip.keyframes.size() - 1; i++)
        {
            _float fNormalizedTime = clip.keyframes[i].fTime / clip.fDuration;
            _float fNextNormalizedTime = clip.keyframes[i + 1].fTime / clip.fDuration;

            if (fRatio >= fNormalizedTime && fRatio <= fNextNormalizedTime)
            {
                iCurrentKeyIdx = i;
                iNextKeyIdx = i + 1;
                break;
            }
        }

        // 키프레임 사이 보간
        if (iCurrentKeyIdx >= 0 && iNextKeyIdx >= 0)
        {
            const UI_KEYFRAME& fromKey = clip.keyframes[iCurrentKeyIdx];
            const UI_KEYFRAME& toKey = clip.keyframes[iNextKeyIdx];

            _float fFromTime = fromKey.fTime / clip.fDuration;
            _float fToTime = toKey.fTime / clip.fDuration;
            _float fLocalRatio = (fRatio - fFromTime) / (fToTime - fFromTime);
            fLocalRatio = clamp(fLocalRatio, 0.f, 1.f);

            // 이징 적용
            _float fEaseRatio = Math::ApplyEase(fromKey.easeType, fLocalRatio);

            // 보간된 값 적용
            _float2 vScale = Math::Lerp(fromKey.vScale, toKey.vScale, fEaseRatio);
            _float fAngle = Math::Lerp(fromKey.fAngle, toKey.fAngle, fEaseRatio);
            _float2 vPosition = Math::Lerp(fromKey.vPosition, toKey.vPosition, fEaseRatio);
            _float4 vColor = {};
            XMStoreFloat4(&vColor, XMVectorLerp(XMLoadFloat4(&fromKey.vColor), XMLoadFloat4(&toKey.vColor), fEaseRatio));

            // UI 오브젝트에 적용
            m_vScale = vScale;
            m_fRadian = XMConvertToRadians(fAngle);

            // 포지션
            //m_vAnchorOffset.x = m_vAnchorOffset.x + XMVectorGetX(vInterpolatedPos);
            // 컬러
        }
        else if (fRatio >= 1.f)
        {
            // 마지막 키프레임 적용
            if (!clip.keyframes.empty())
            {
                const UI_KEYFRAME& lastKey = clip.keyframes.back();
                m_vScale.x = lastKey.vScale.x;
                m_vScale.y = lastKey.vScale.y;
                m_fRadian = XMConvertToRadians(lastKey.fAngle);
            }
        }

        // 애니메이션 종료 처리
        if (fRatio >= 1.f)
        {
            if (!clip.isLoop)
                m_isBlending = false;

            m_fBlendTime = 0.f;
        }
    }
    else
    {
        // 애니메이션 재생이 아닐 때 리셋을 어디서 해줘야하지
    }
}

void CUIObject_Tool::Set_Animation(_uint iIndex)
{
    if (m_iCurrentClipIndex == iIndex)//&& m_isAnimLoop == isLoop)
        return;

    m_iCurrentClipIndex = iIndex;

    //m_iCurrentClipIndex[iIndex]->Reset();
}

void CUIObject_Tool::Change_Texture(_uint index, const string& levelKey, const string& TextureKey, string& OutstrTextureKey)
{
    Get_Component<CSprite2D>()->Change_Texture(index, levelKey, TextureKey);
    OutstrTextureKey = TextureKey;
}

void CUIObject_Tool::Free()
{
    __super::Free();
}