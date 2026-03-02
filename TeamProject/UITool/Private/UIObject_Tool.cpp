#include "pch.h"
#include "UIObject_Tool.h"

#include "GameInstance.h"
#include "ObjectContainer.h"
#include "Sprite2D.h"
#include "Texture.h"
#include "Helper_Func.h"
#include "Child.h"

HRESULT CUIObject_Tool::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);

    auto sprite = Get_Component<CSprite2D>();

    sprite->Set_Param("vColor", {&m_vColorLinear, "float4", sizeof(_float4)});
    sprite->Add_Texture(G_GlobalLevelKey, "empty.png");

    m_colorTexModeU = (_uint)m_colorTexMode;
    sprite->Set_Param("ColorTexMode", {&m_colorTexModeU, "uint", sizeof(_uint)});
    sprite->Set_Param("ColorTexMix", {&m_colorTexMix, "float", sizeof(_float)});

    sprite->Set_Param("ColorUVUse", {&m_colorUVUseU, "uint", sizeof(_uint)});
    sprite->Set_Param("ColorUVOffset", {&m_colorUVOffset, "float2", sizeof(Vector2)});
    sprite->Set_Param("ColorUVScale", {&m_colorUVScale, "float2", sizeof(Vector2)});

    GUISystem()->Get_Context()->pSelectedObject = this;
    return S_OK;
}

void CUIObject_Tool::Awake()
{
    __super::Awake();

    m_vAnchorOffset = Get_AnchorOffset(m_eAnchor);
}

void CUIObject_Tool::Update(_float dt)
{
    __super::Update(dt);

    KeyInput_ReorderChildren();

    if (m_colorUVAutoScroll && m_colorUVUseU != 0)
    {
        m_colorUVOffset += m_colorUVSpeed * dt;

        m_colorUVOffset.x -= floorf(m_colorUVOffset.x);
        m_colorUVOffset.y -= floorf(m_colorUVOffset.y);

        auto sprite = Get_Component<CSprite2D>();
        sprite->Set_Param("ColorUVOffset", {&m_colorUVOffset, "float2", sizeof(Vector2)});
    }
}

void CUIObject_Tool::Render_GUI()
{
    if (auto pContainer = Get_Component<CObjectContainer>())
        pContainer->Render_GUI();

    Render_GUI_Property();
    Render_GUI_Layout();
    Render_GUI_Transform();
    Render_GUI_Color();
    Render_GUI_Animation();
}

void CUIObject_Tool::OnClick()
{
    auto pGuiSystem = CGameInstance::GetInstance()->Get_GUISystem();

    if (!pGuiSystem->UsingUI())
        pGuiSystem->Get_Context()->pSelectedObject = this;
}

void CUIObject_Tool::Remove_SelfFromParent()
{
    const string& strCurrentLevel = LevelManager()->Get_NowLevelKey();
    const auto& objects = UIManager()->Get_LevelUI(strCurrentLevel);

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

void CUIObject_Tool::Save(nlohmann::ordered_json& data)
{
    data["alive"] = m_isAlive;
    data["instanceName"] = m_InstanceName;

    auto& transformJson = data["transform"];
    transformJson["anchor"] = ENUM(m_eAnchor);
    transformJson["anchorOffset"] = {m_vAnchorOffset.x, m_vAnchorOffset.y};
    transformJson["size"] = {m_vSize.x, m_vSize.y};
    transformJson["scale"] = {m_vScale.x, m_vScale.y};
    transformJson["pivot"] = {m_vPivot.x, m_vPivot.y};
    transformJson["radian"] = m_fRadian;

    data["color"] = {m_vColor.x, m_vColor.y, m_vColor.z, m_vColor.w};

    data["stencilMode"] = m_useMask ? ENUM(StencilMode::Test) : ENUM(StencilMode::None);
    data["pass"] = Get_Component<CSprite2D>()->Get_PassConstant();

    m_colorTexModeU = (_uint)m_colorTexMode;
    data["colorTexKey"] = m_colorTextureKey;
    data["colorTexMode"] = m_colorTexModeU;
    data["colorTexMix"] = m_colorTexMix;

    data["colorUVUse"] = m_colorUVUseU;
    data["colorUVOffset"] = {m_colorUVOffset.x, m_colorUVOffset.y};
    data["colorUVScale"] = {m_colorUVScale.x, m_colorUVScale.y};
    data["colorUVAutoScroll"] = m_colorUVAutoScroll;
    data["colorUVSpeed"] = {m_colorUVSpeed.x, m_colorUVSpeed.y};

    auto& animClipsJson = data["animClips"];
    animClipsJson = json::array();
    for (const auto& clip : m_AnimClips)
    {
        json clipData{};
        clipData["name"] = clip.strName;
        clipData["duration"] = clip.fDuration;
        clipData["loop"] = clip.isLoop;

        auto& keyframesJson = clipData["keyframes"];
        keyframesJson = json::array();
        for (const auto& keyframe : clip.keyframes)
        {
            json keyframeData{};
            keyframeData["time"] = keyframe.fTime;
            keyframeData["scale"] = {keyframe.vScale.x, keyframe.vScale.y};
            keyframeData["angle"] = keyframe.fAngle;
            keyframeData["position"] = {keyframe.vPosition.x, keyframe.vPosition.y};
            keyframeData["color"] = {keyframe.vColor.x, keyframe.vColor.y, keyframe.vColor.z, keyframe.vColor.w};
            keyframeData["easeType"] = ENUM(keyframe.easeType);

            keyframesJson.push_back(keyframeData);
        }
        animClipsJson.push_back(clipData);
    }

    auto& childrenJson = data["children"];
    childrenJson = json::array();

    auto pContainer = Get_Component<CObjectContainer>();
    if (!pContainer) return;

    const auto& children = pContainer->Get_ChildrenByOrder();
    for (auto& pChild : children)
    {
        auto pChildUI = dynamic_cast<CUIObject_Tool*>(pChild);
        if (!pChildUI) continue;

        nlohmann::ordered_json childData{};
        pChildUI->Save(childData);
        childrenJson.push_back(childData);
    }
}

void CUIObject_Tool::Load(const nlohmann::ordered_json& data)
{
    __super::Load(data);

    const string pass = Get_Component<CSprite2D>()->Get_PassConstant();

    m_basePass = NormalizeToBasePass(pass);
    m_useMask = (pass != m_basePass);

    m_colorTextureKey = data.value("colorTexKey", string("empty.png"));
    m_colorTexModeU = (_uint)data.value("colorTexMode", 0u);
    m_colorTexMix = (_float)data.value("colorTexMix", 1.f);

    m_colorTexMode = (UIColorTexMode)m_colorTexModeU;

    m_colorUVUseU = (_uint)data.value("colorUVUse", 0u);
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

    auto sprite = Get_Component<CSprite2D>();
    sprite->Change_Texture(1, G_GlobalLevelKey, m_colorTextureKey);

    sprite->Set_Param("ColorTexMode", {&m_colorTexModeU, "uint", sizeof(_uint)});
    sprite->Set_Param("ColorTexMix", {&m_colorTexMix, "float", sizeof(_float)});
    sprite->Set_Param("ColorUVUse", {&m_colorUVUseU, "uint", sizeof(_uint)});
    sprite->Set_Param("ColorUVOffset", {&m_colorUVOffset, "float2", sizeof(Vector2)});
    sprite->Set_Param("ColorUVScale", {&m_colorUVScale, "float2", sizeof(Vector2)});
}

void CUIObject_Tool::Render_GUI_Property()
{
    ImGui::SeparatorText(u8"속성");
    ImGui::Checkbox("Alive", &m_isAlive);

    _char szInstanceName[MAX_PATH] = {};
    strcpy_s(szInstanceName, m_InstanceName.c_str());
    if (ImGui::InputText(u8"인스턴스네임", szInstanceName, sizeof(szInstanceName)))
        m_InstanceName = szInstanceName;

    if (ImGui::Checkbox("Use Mask", &m_useMask))
    {
        m_stencilMode = m_useMask? StencilMode::Test : StencilMode::None;
        Set_BasePass(m_basePass);
    }

    ImGui::TextDisabled(("BasePass : " + m_basePass).c_str());
}

void CUIObject_Tool::Render_GUI_Layout()
{
    ImGui::SeparatorText("Layout");

    ImGui::Text("Anchor");
    struct AnchorPreset { const char* label; ANCHOR value; };
    static const AnchorPreset presets[9] =
    {
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
        {
            m_eAnchor = presets[i].value;
            m_vAnchorOffset = Get_AnchorOffset(m_eAnchor);  // 앵커에 맞춰서 자동 정렬
        } 

        if (selected) ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    ImGui::TextDisabled("Selected: %u", (_uint)m_eAnchor);

    ImGui::DragFloat2(u8"위치", reinterpret_cast<_float*>(&m_vAnchorOffset));
    // -----------------------------------------------------------------------
    
    Render_GUI_SizeBlock();
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

    if (m_AnimClips.empty())
        return;

    string strCombined;
    for (_int i = 0; i < m_AnimClips.size(); ++i)
        strCombined += m_AnimClips[i].strName + '\0';
    strCombined += '\0';

    ImGui::Combo(u8"클립", &m_iClipIndex, strCombined.c_str());

    if (-1 == m_iClipIndex)
        return;

    if (ImGui::Button(u8"클립 편집"))
    {
        if (m_iClipIndex >= 0 && m_iClipIndex < m_AnimClips.size())
            ImGui::OpenPopup("clipEditor");
    }

    ImGui::SetNextWindowPos(ImVec2(g_iWinSizeX * 0.7f, 100), ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(ImVec2(240.f, g_iWinSizeY * 0.8f), ImGuiCond_Appearing);

    if (ImGui::BeginPopup("clipEditor"))
    {
        ImVec2 avail = ImGui::GetContentRegionAvail();
        ImGui::BeginChild("Content", avail, false, ImGuiWindowFlags_AlwaysVerticalScrollbar);

        UI_ANIM_CLIP& clip = m_AnimClips[m_iClipIndex];

        ImGui::SeparatorText(u8"재생");
        ImGui::BeginDisabled(clip.keyframes.empty());
        if (ImGui::Button(m_isBlending ? u8"정지" : u8"재생"))
        {
            m_isBlending = !m_isBlending;
            if (m_isBlending)
                Set_Animation(m_iClipIndex, clip.isLoop);
            else
                m_isBlending = false;
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        if (m_isBlending)   ImGui::TextColored(ImVec4(0.2f, 1.f, 0.2f, 1.f), u8"● Playing");
        else                ImGui::TextColored(ImVec4(1.f, 0.3f, 0.3f, 1.f), u8"■ Stopped");

        {
            const _float duration = max(clip.fDuration, 0.0001f);

            if (m_isBlending && !m_timelineDragging)
                m_timelineTime = clamp(m_fBlendTime, 0.f, duration);

            _float curTime = m_isBlending ? clamp(m_fBlendTime, 0.f, duration) : clamp(m_timelineTime, 0.f, duration);

            const float w = ImGui::GetContentRegionAvail().x;
            const float h = 34.f;

            ImVec2 p0 = ImGui::GetCursorScreenPos();
            ImVec2 p1 = ImVec2(p0.x + w, p0.y + h);

            ImGui::InvisibleButton("##timeline", ImVec2(w, h));
            const bool hovered = ImGui::IsItemHovered();
            const bool active = ImGui::IsItemActive();

            auto draw = ImGui::GetWindowDrawList();
            const ImU32 colBg = ImGui::GetColorU32(ImGuiCol_FrameBg);
            const ImU32 colBd = ImGui::GetColorU32(ImGuiCol_Border);
            const ImU32 colTick = ImGui::GetColorU32(ImGuiCol_TextDisabled);
            const ImU32 colHead = ImGui::GetColorU32(ImGuiCol_SliderGrabActive);

            draw->AddRectFilled(p0, p1, colBg, 6.f);
            draw->AddRect(p0, p1, colBd, 6.f);

            for (auto& kf : clip.keyframes)
            {
                float x = p0.x + (kf.fTime / duration) * w;
                draw->AddLine(ImVec2(x, p0.y + 4.f), ImVec2(x, p1.y - 4.f), colTick);
            }

            float headX = p0.x + (curTime / duration) * w;
            draw->AddLine(ImVec2(headX, p0.y + 2.f), ImVec2(headX, p1.y - 2.f), colHead, 2.f);

            bool changed = false;

            if (hovered && ImGui::IsMouseClicked(0))
            {
                m_timelineDragging = true;
                m_timelineResumePlay = m_isBlending;
                m_isBlending = false;
            }

            if (m_timelineDragging)
            {
                float mx = ImGui::GetIO().MousePos.x;
                mx = clamp(mx, p0.x, p1.x);
                curTime = (mx - p0.x) / w * duration;
                changed = true;

                if (!ImGui::IsMouseDown(0))
                {
                    m_timelineDragging = false;
                    if (m_timelineResumePlay)
                    {
                        m_isBlending = true;
                        m_fBlendTime = curTime;
                    }
                }
            }

            if (changed)
            {
                m_timelineTime = curTime;

                const bool prevBlending = m_isBlending;
                if (!prevBlending)
                {
                    m_isBlending = true;
                    Set_Animation(m_iClipIndex, clip.isLoop);
                    m_fBlendTime = curTime;
                    Play_Animation(0.f);
                    m_isBlending = false;
                }
            }

            ImGui::TextDisabled("t %.3f / %.3f", curTime, duration);

            if (hovered)
            {
                float mx = clamp(ImGui::GetIO().MousePos.x, p0.x, p1.x);
                _float hoverTime = (mx - p0.x) / w * duration;
                ImGui::SetTooltip("t %.3f", hoverTime);
            }

            ImGui::Spacing();
        }

        ImGui::SeparatorText(u8"기본 속성 편집");
        char szBuffer[256] = {};
        strcpy_s(szBuffer, clip.strName.c_str());
        if (ImGui::InputText(u8"이름", szBuffer, sizeof(szBuffer)))
            clip.strName = szBuffer;
        ImGui::InputFloat(u8"길이", &clip.fDuration);
        ImGui::Checkbox(u8"루프", &clip.isLoop);

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
            if (!clip.keyframes.empty())
                clip.keyframes.pop_back();
        }

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

        ImGui::EndChild();
        ImGui::EndPopup();
    }
}

void CUIObject_Tool::Render_GUI_Color()
{
    ImGui::SeparatorText(u8"컬러");

    bool dirty = false;

    if (ImGui::ColorEdit4(u8"##ColorPick", reinterpret_cast<_float*>(&m_vColor)))
        dirty = true;

    const float panelW = ImGui::GetContentRegionAvail().x;
    const float labelW = clamp(panelW * 0.38f, 90.f, 150.f);
    const float rightPad = 10.f;

    auto Row = [&](const char* label, auto&& widget)
        {
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted(label);
            ImGui::SameLine(labelW);

            float w = ImGui::GetContentRegionAvail().x - rightPad;
            if (w < 60.f) w = 60.f;
            ImGui::SetNextItemWidth(w);

            widget();
        };

    static const char* kModes[] = {"None", "Replace", "Multiply"};
    int mode = (int)m_colorTexMode;

    Row(u8"컬러 텍스처", [&]
        {
            float w = min(140.f, ImGui::GetContentRegionAvail().x - rightPad);
            if (w < 80.f) w = 80.f;
            ImGui::SetNextItemWidth(w);

            if (ImGui::Combo(u8"##ColorTexMode", &mode, kModes, IM_ARRAYSIZE(kModes)))
            {
                m_colorTexMode = (UIColorTexMode)mode;
                dirty = true;
            }
        });

    if (m_colorTexMode == UIColorTexMode::None)
    {
        if (dirty)
        {
            auto sprite = Get_Component<CSprite2D>();
            m_colorTexModeU = (_uint)m_colorTexMode;
            sprite->Set_Param("ColorTexMode", {&m_colorTexModeU, "uint", sizeof(_uint)});
            sprite->Set_Param("ColorTexMix", {&m_colorTexMix, "float", sizeof(_float)});
        }
        return;
    }

    ImGui::Spacing();

    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 6.f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.f, 10.f));
    ImGui::BeginChild("##ColorTexCard", ImVec2(0.f, 0.f), true, ImGuiWindowFlags_None);

    ImGui::SeparatorText(u8"텍스처");

    Row(u8"텍스처", [&]
        {
            if (ImGui::Button(u8"선택", ImVec2(64.f, 0.f)))
            {
                string filePath = Helper::OpenFile({{"PNG Files", "*.png"}}, "png");
                if (!filePath.empty())
                {
                    string fileName = Helper::GetFileNameWithExtension(filePath);
                    CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath(fileName, filePath);

                    m_colorTextureKey = fileName;

                    auto sprite = Get_Component<CSprite2D>();
                    sprite->Change_Texture(1, G_GlobalLevelKey, m_colorTextureKey);

                    dirty = true;
                }
            }

            ImGui::SameLine(0.f, 8.f);

            string view = m_colorTextureKey.empty() ? string("(none)") : m_colorTextureKey;
            if (!m_colorTextureKey.empty() && view.size() > 24)
                view = view.substr(0, 21) + "...";

            ImGui::TextDisabled("%s", view.c_str());
            if (!m_colorTextureKey.empty() && ImGui::IsItemHovered())
                ImGui::SetTooltip("%s", m_colorTextureKey.c_str());
        });

    Row(u8"Mix", [&]
        {
            float w = min(120.f, ImGui::GetContentRegionAvail().x - rightPad);
            if (w < 90.f) w = 90.f;
            ImGui::SetNextItemWidth(w);

            if (ImGui::DragFloat(u8"##Mix", &m_colorTexMix, 0.01f, 0.f, 1.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
                dirty = true;
        });

    ImGui::Spacing();
    ImGui::SeparatorText(u8"UV");

    Row(u8"사용", [&]
        {
            bool use = (m_colorUVUseU != 0);
            if (ImGui::Checkbox(u8"##ColorUVUse", &use))
            {
                m_colorUVUseU = use ? 1u : 0u;
                dirty = true;
            }
        });

    ImGui::BeginDisabled(m_colorUVUseU == 0);

    Row(u8"Offset", [&]
        {
            if (ImGui::DragFloat2(u8"##ColorUVOffset", reinterpret_cast<_float*>(&m_colorUVOffset), 0.01f, -10.f, 10.f, "%.3f"))
                dirty = true;
        });

    Row(u8"Scale", [&]
        {
            if (ImGui::DragFloat2(u8"##ColorUVScale", reinterpret_cast<_float*>(&m_colorUVScale), 0.01f, 0.01f, 50.f, "%.3f"))
                dirty = true;
        });

    Row(u8"AutoScroll", [&]
        {
            if (ImGui::Checkbox(u8"##ColorUVAutoScroll", &m_colorUVAutoScroll))
                dirty = true;
        });

    ImGui::BeginDisabled(!m_colorUVAutoScroll);

    Row(u8"Speed", [&]
        {
            if (ImGui::DragFloat2(u8"##ColorUVSpeed", reinterpret_cast<_float*>(&m_colorUVSpeed), 0.01f, -10.f, 10.f, "%.3f"))
                dirty = true;
        });

    ImGui::EndDisabled();
    ImGui::EndDisabled();

    ImGui::Spacing();

    ImGui::EndChild();
    ImGui::PopStyleVar(2);

    if (!dirty)
        return;

    auto sprite = Get_Component<CSprite2D>();

    m_colorTexModeU = (_uint)m_colorTexMode;
    sprite->Set_Param("ColorTexMode", {&m_colorTexModeU, "uint", sizeof(_uint)});
    sprite->Set_Param("ColorTexMix", {&m_colorTexMix, "float", sizeof(_float)});

    sprite->Set_Param("ColorUVUse", {&m_colorUVUseU, "uint", sizeof(_uint)});
    sprite->Set_Param("ColorUVOffset", {&m_colorUVOffset, "float2", sizeof(Vector2)});
    sprite->Set_Param("ColorUVScale", {&m_colorUVScale, "float2", sizeof(Vector2)});
}

_bool CUIObject_Tool::Render_GUI_Image(string& strTextureKey)
{
    _bool isDirty = {};

    ImGui::SeparatorText(u8"이미지");
    if (ImGui::Button(u8"선택"))
    {
        string filePath = Helper::OpenFile({{"PNG Files", "*.png"}}, "png");
        if (filePath.empty())
            return isDirty;

        string fileName = Helper::GetFileNameWithExtension(filePath);

        ResourceManager()->Add_ResourcePath(fileName, filePath);

        strTextureKey = fileName;

        ApplySpriteTexture(0, G_GlobalLevelKey, strTextureKey, true);

        m_vAnchorOffset = Get_AnchorOffset(m_eAnchor);

        isDirty = true;
    }

    auto sprite = Get_Component<CSprite2D>();
    sprite->Render_GUI();

    const string edited = NormalizeToBasePass(sprite->Get_PassConstant());
    if (edited != m_basePass)
        Set_BasePass(edited);

    return isDirty;
}

void CUIObject_Tool::ApplySpriteTexture(_uint idx, const string& levelKey, const string& texKey, _bool applyOriginSize)
{
    auto sprite = Get_Component<CSprite2D>();
    sprite->Change_Texture(idx, levelKey, texKey);

    m_sizeMode = UISizeMode::FHD;

    if (sprite->IsValid())
    {
        auto texture = sprite->Get_Texture(idx);
        auto size    = texture->Get_Size();

        m_sizeFHD = {(float)size.x, (float)size.y};
    }

    if (!applyOriginSize || !Get_OriginTexSize()) return;
    const _float ratio = GetSizeRatio(m_sizeMode);
    Set_Size({m_sizeFHD.x * ratio, m_sizeFHD.y * ratio});
}

_float2 CUIObject_Tool::Get_AnchorOffset(ANCHOR eAnchor)
{
    _uint iAnchor = static_cast<_uint>(eAnchor);
    _float2 fOffset = {};

    if (iAnchor & static_cast<_uint>(ANCHOR::Right))
        fOffset.x = m_vSize.x * -1.f;
    else if (iAnchor & static_cast<_uint>(ANCHOR::Left))
        fOffset.x = 0.f;
    else
        fOffset.x = m_vSize.x * - 0.5f;

    if (iAnchor & static_cast<_uint>(ANCHOR::Bottom))
        fOffset.y = m_vSize.y * -1.f;
    else if (iAnchor & static_cast<_uint>(ANCHOR::Top))
        fOffset.y = 0.f;
    else
        fOffset.y = m_vSize.y * -0.5f;

    return fOffset;
}

void CUIObject_Tool::KeyInput_ReorderChildren()
{
    auto pGameInstance = CGameInstance::GetInstance();

    auto pInput = pGameInstance->Get_InputDev();
    auto pSelected = pGameInstance->Get_GUISystem()->Get_Context()->pSelectedObject;
    auto pChild = Get_Component<CChild>();

    if (!pChild || !pSelected || (pSelected != this))
        return;

    // 상단 ~ 하단 순서대로 그려짐
    if (pInput->Key_Hold(VK_CONTROL))
    {
        if (pInput->Key_Tap(VK_OEM_4))      //  ctrl + [ : 최상단으로 이동
            pChild->Set_Order_First(this);
        else if (pInput->Key_Tap(VK_OEM_6)) //  ctrl + ] : 최하단으로 이동
            pChild->Set_Order_Last(this);
    }
    else
    {
        if (pInput->Key_Tap(VK_OEM_4))      //  [ : 한 단계 위로 이동
            pChild->Upper_Order(this);
        else if (pInput->Key_Tap(VK_OEM_6)) //  ] : 한 단계 아래로 이동
        {
            OutputDebugString(Helper::ConvertToWideString(m_InstanceName).c_str());
            pChild->Lower_Order(this);
        }
    }
}

void CUIObject_Tool::Set_BasePass(const string& pass)
{
    m_basePass = pass;

    auto sprite = Get_Component<CSprite2D>();
    if (m_useMask) sprite->ChangePass(MapToStencilTestPass(m_basePass));
    else           sprite->ChangePass(m_basePass);
}

string CUIObject_Tool::MapToStencilTestPass(const string& basePass)
{
    if (basePass == "Opaque")          return "Opaque_StencilTest";
    if (basePass == "UVAnimation")     return "UVAnimation_StencilTest";
    if (basePass == "LinearFill")      return "LinearFill_StencilTest";
    if (basePass == "RadialFill")      return "RadialFill_StencilTest";
    if (basePass == "SpriteAnimation") return "SpriteAnimation_StencilTest";
    return basePass;
}

string CUIObject_Tool::NormalizeToBasePass(const string& pass)
{
    if (pass == "Opaque_StencilTest")          return "Opaque";
    if (pass == "UVAnimation_StencilTest")     return "UVAnimation";
    if (pass == "LinearFill_StencilTest")      return "LinearFill";
    if (pass == "RadialFill_StencilTest")      return "RadialFill";
    if (pass == "SpriteAnimation_StencilTest") return "SpriteAnimation";
    return pass;
}

_float CUIObject_Tool::GetSizeRatio(UISizeMode mode)
{
    if      (mode == UISizeMode::Default) return 1.f;
    else if (mode == UISizeMode::QHD)     return g_iWinSizeX / 2560.f;
    else if (mode == UISizeMode::UHD)     return g_iWinSizeX / 3840.f;
    else                                  return g_iWinSizeX / 1920.f;
}

void CUIObject_Tool::Render_GUI_SizeBlock()
{
    const _float curRatio = GetSizeRatio(m_sizeMode);

    if (m_sizeFHD.x == 0.f && m_sizeFHD.y == 0.f)
        m_sizeFHD = {m_vSize.x / curRatio, m_vSize.y / curRatio};
     
    auto sprite = Get_Component<CSprite2D>();
    _uint2 vSize{};
    if (sprite->IsValid())
       vSize = sprite->Get_Texture(0)->Get_Size();

    float fAspectRatio = vSize.x / max(static_cast<_float>(vSize.y), 1.f);

    ImGui::Checkbox(u8"##lock", &m_isAspectRatioLocked);
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
        ImGui::SetTooltip(u8"종횡비 고정\n텍스쳐 가로/세로 비율로 유지합니다");

    ImGui::SameLine();
    ImGui::PushItemWidth(64.f);
    if (ImGui::DragFloat(u8"##x", &m_vSize.x, 1.f, 0.f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp))
    {
        if (m_isAspectRatioLocked)
            m_vSize.y = m_vSize.x / fAspectRatio;
    }
    ImGui::SameLine();
    if (ImGui::DragFloat(u8"##y", &m_vSize.y, 1.f, 0.f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp))
    {
        if (m_isAspectRatioLocked)
            m_vSize.x = m_vSize.y * fAspectRatio;
    }
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::Text(u8"사이즈");

    static const char* kModes[] =
    {
        "Default",
        "FHD (1920x1080)",
        "QHD (2560x1440)",
        "UHD (3840x2160)"
    };

    int mode = (int)m_sizeMode;
    if (ImGui::Combo(u8"기준 해상도", &mode, kModes, IM_ARRAYSIZE(kModes)))
    {
        m_sizeMode = (UISizeMode)mode;
        const _float newRatio = GetSizeRatio(m_sizeMode);
        m_vSize = {m_sizeFHD.x * newRatio, m_sizeFHD.y * newRatio};
    }
}