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

    Get_Component<CSprite2D>()->Set_Param("vColor", { &m_vColor, "float4",sizeof(_float4) });

    // GUI Inspector 창에 띄움
    CGameInstance::GetInstance()->Get_GUISystem()->Get_Context()->pSelectedObject = this;

    return S_OK;
}

void CUIObject_Tool::Render_GUI()
{
    ImGui::SeparatorText(u8"속성");
    ImGui::Checkbox("Alive", &m_isAlive);

    Render_GUI_Layout();

    Render_GUI_Transform();

    Render_GUI_Color();

    Render_GUI_Animation();
}

void CUIObject_Tool::Remove_SelfFromParent()
{
    const string& strCurrentLevel = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();
    const auto& objects = CGameInstance::GetInstance()->Get_UIMgr()->Get_LevelUI(strCurrentLevel); 

    for (auto& pObj : objects)
    {
        if (!pObj || pObj == this)
            continue;

        CObjectContainer* pContainer = pObj->Get_Component<CObjectContainer>();
        if (!pContainer)
            continue;

        const auto& children = pContainer->Get_Children();
        for (_int i = 0; i < children.size(); ++i)
        {
            if (!children[i])
                continue;

            if (children[i] == this)
            {
                pContainer->Destroy_Child(i);
                return;
            }
        }
    }
}

void CUIObject_Tool::FillElementData(UI_ELEMENT_DATA& data)
{
    // 공통 데이터 저장
    data.transform.iAnchor = static_cast<_uint>(m_eAnchor);
    data.transform.vAnchorOffset = { m_vAnchorOffset.x, m_vAnchorOffset.y };
    data.transform.vSize = { m_vSize.x, m_vSize.y };
    data.transform.vScale = { m_vScale.x, m_vScale.y };
    data.transform.vPivot = { m_vPivot.x, m_vPivot.y };
    data.transform.fRadian = m_fRadian;
    data.vColor = { m_vColor.x, m_vColor.y, m_vColor.z, m_vColor.w };

    // 애니메이션 데이터 저장
    for (auto& clip : m_AnimClips)
    {
        UI_CLIP_DATA clipData = {};

        clipData.strName = clip.strName;
        clipData.fDuration = clip.fDuration;
        clipData.isLoop = clip.isLoop;

        for (auto& keyframe : clip.keyframes)
        {
            UI_KEYFRAME_DATA keyframeData = {};
            keyframeData.fTime = keyframe.fTime;
            keyframeData.vScale = { keyframe.vScale.x, keyframe.vScale.y};
            keyframeData.fAngle = keyframe.fAngle;
            keyframeData.vPosition = { keyframe.vPosition.x, keyframe.vPosition.y };
            keyframeData.vColor = { keyframe.vColor.x, keyframe.vColor.y, keyframe.vColor.z, keyframe.vColor.w };
            keyframeData.uEaseType = static_cast<_uint>(keyframe.easeType);

            clipData.keyframes.push_back(keyframeData);
        }
        data.animClips.push_back(clipData);
    } 

    // 자식 데이터 저장
    CObjectContainer* pContainer = Get_Component<CObjectContainer>();
    if (pContainer && !pContainer->Get_Children().empty())
    {
        const auto& children = Get_Component<CObjectContainer>()->Get_Children();
        for (auto& pChild : children)
        {
            CUIObject_Tool* pChildUI = dynamic_cast<CUIObject_Tool*>(pChild);
            if (!pChildUI)
                continue;

            UI_ELEMENT_DATA childElementData = {};
            pChildUI->FillElementData(childElementData);
            data.children.push_back(childElementData);
        }
    }
}

void CUIObject_Tool::ReadElementData(const UI_ELEMENT_DATA& data)
{
    // 공통 데이터 읽기
    m_eAnchor = static_cast<ANCHOR>(data.transform.iAnchor);
    m_vAnchorOffset = _float2(data.transform.vAnchorOffset[0], data.transform.vAnchorOffset[1]);
    m_vSize = _float2(data.transform.vSize[0], data.transform.vSize[1]);
    m_vScale = _float2(data.transform.vScale[0], data.transform.vScale[1]);
    m_vPivot = _float2(data.transform.vPivot[0], data.transform.vPivot[1]);
    m_fRadian = data.transform.fRadian;
    m_vColor = _float4(data.vColor[0], data.vColor[1], data.vColor[2], data.vColor[3]);

    // 애니메이션 데이터 읽기
    for (auto& clipData : data.animClips)
    {
        UI_ANIM_CLIP clip = { clipData.strName };
        clip.fDuration = clipData.fDuration;
        clip.isLoop = clipData.isLoop;

        for (auto& keyframeData : clipData.keyframes)
        {
            UI_KEYFRAME keyframe = {};

            keyframe.fTime = keyframeData.fTime;
            keyframe.vScale = { keyframeData.vScale[0], keyframeData.vScale[1] };
            keyframe.fAngle = keyframeData.fAngle;
            keyframe.vPosition = { keyframeData.vPosition[0], keyframeData.vPosition[1] };
            keyframe.vColor = { keyframeData.vColor[0], keyframeData.vColor[1], keyframeData.vColor[2], keyframeData.vColor[3] };
            keyframe.easeType = static_cast<EaseType>(keyframeData.uEaseType);

            clip.keyframes.push_back(keyframe);
        }
        m_AnimClips.push_back(clip);
    }

    // 자식 데이터 읽기
    CObjectContainer* pContainer = Get_Component<CObjectContainer>();
    if (pContainer)
    {
        for (auto& childElementData : data.children)
        {
            const string& strCurrentLevelKey = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();
            const string& strTypeTag = childElementData.strTypeTag;
            CUI_Object* pChildObj = Builder::Create_UIObject({ strCurrentLevelKey , "Proto_GameObject_" + strTypeTag })
                .Build(strTypeTag);

            if (!pChildObj)
                continue;

            CUIObject_Tool* pChildUI = dynamic_cast<CUIObject_Tool*>(pChildObj);
            if (pChildUI)
                pChildUI->ReadElementData(childElementData);

            pContainer->Add_Child(pChildUI);
        }
    } 
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

    ImGui::TextDisabled("AnchorOffset : %.1f, %.1f", m_vAnchorOffset.x, m_vAnchorOffset.y);
    ImGui::TextDisabled("ScreenOffset : %.1f, %.1f", m_vScreenOffset.x, m_vScreenOffset.y);
    ImGui::TextDisabled("LeftTop : %.1f, %.1f", m_vLeftTop.x, m_vLeftTop.y);
    ImGui::TextDisabled("WinSize : %.1f x %.1f", m_WinSize.x, m_WinSize.y);
}

void CUIObject_Tool::Render_GUI_Animation()
{
    ImGui::SeparatorText(u8"애니메이션");

    // 클립 추가
    {
        ImGui::AlignTextToFramePadding();
        ImGui::Text((u8"클립 (" + to_string(m_AnimClips.size()) + ")").c_str());

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
        UI_ANIM_CLIP& clip = m_AnimClips[iClipIndex];

        ImGui::SetNextWindowPos(ImVec2(g_iWinSizeX * 0.72f, 100), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(200, g_iWinSizeY * 0.8f), ImGuiCond_Once);
        ImGui::Begin(clip.strName.c_str(), &showPopup);

        // 재생, 정지 
        ImGui::SeparatorText(u8"재생");
        ImGui::BeginDisabled(clip.keyframes.empty());
        if (ImGui::Button(m_isBlending ? u8"정지" : u8"재생"))
        {
            m_isBlending = !m_isBlending;
            if (m_isBlending)
                Set_Animation(iClipIndex);
            else
                Reset_Animation();
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        if (m_isBlending)   ImGui::TextColored(ImVec4(0.2f, 1.f, 0.2f, 1.f), u8"● Playing");
        else                ImGui::TextColored(ImVec4(1.f, 0.3f, 0.3f, 1.f), u8"■ Stopped"); 

        // 선택한 클립 편집
        ImGui::SeparatorText(u8"기본 속성 편집");
        char szBuffer[256] = {};
        strcpy_s(szBuffer, clip.strName.c_str());
        if (ImGui::InputText(u8"이름", szBuffer, sizeof(szBuffer)))
            clip.strName = szBuffer;
        ImGui::InputFloat(u8"길이", &clip.fDuration);
        ImGui::Checkbox(u8"루프", &clip.isLoop);

        // 키프레임 추가
        ImGui::SeparatorText((u8"키프레임 ( " + to_string(clip.keyframes.size()) + " )").c_str());
        if (ImGui::Button(u8"추가 +"))
        {
            _float fTime = {};
            if (!clip.keyframes.empty())
                fTime = min(clip.keyframes.back().fTime + 0.5f, clip.fDuration);

            clip.keyframes.push_back(UI_KEYFRAME(fTime));
        }
        ImGui::SameLine();
        if (ImGui::Button(u8"삭제 -"))
        {
            if(!clip.keyframes.empty())
                clip.keyframes.pop_back();
        }

        // 키프레임 편집 
        ImGui::Separator();
        int idx = 0;
        for (auto& keyframe : clip.keyframes)
        {
            ImGui::PushID(idx);

            if (ImGui::TreeNodeEx(("Keyframe : " + to_string(keyframe.fTime)).c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_NoTreePushOnOpen))
            {
                ImGui::TreePush("##Content");

                ImGui::DragFloat(u8"시간", &keyframe.fTime, 0.1f, 0.f, clip.fDuration, "%.2f", ImGuiSliderFlags_AlwaysClamp);
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

void CUIObject_Tool::Render_GUI_Color()
{
    ImGui::SeparatorText(u8"컬러");
    ImGui::ColorEdit4(u8"컬러", reinterpret_cast<_float*>(&m_vColor));
}

_int CUIObject_Tool::Find_TextureIndex(const vector<const _char*> TextureKeys, const string strTextureTag)
{
    for (_int i = 0; i < TextureKeys.size(); ++i)
        if (TextureKeys[i] == strTextureTag)
            return i;

    return -1;
}

void CUIObject_Tool::Free()
{
    __super::Free();
}