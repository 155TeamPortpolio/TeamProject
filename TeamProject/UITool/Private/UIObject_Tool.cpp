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

    Get_Component<CSprite2D>()->Set_Param("vColor", {&m_vColorLinear, "float4", sizeof(_float4)});

    // GUI Inspector 창에 띄움
    CGameInstance::GetInstance()->Get_GUISystem()->Get_Context()->pSelectedObject = this;

    return S_OK;
}

void CUIObject_Tool::Awake()
{
    __super::Awake();

    m_vAnchorOffset = Get_AnchorOffset(m_eAnchor);

    Set_Clickable(true);
}

void CUIObject_Tool::Update(_float dt)
{
    KeyInput_ReorderChildren();

    Play_Animation(dt);
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
    auto pGameInstance = CGameInstance::GetInstance();

    const string& strCurrentLevel = pGameInstance->Get_LevelMgr()->Get_NowLevelKey();
    const auto& objects = pGameInstance ->Get_UIMgr()->Get_LevelUI(strCurrentLevel);

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
    data["instanceName"] = m_InstanceName;

    auto& transformJson = data["transform"];
    transformJson["anchor"] = ENUM(m_eAnchor);
    transformJson["anchorOffset"] = { m_vAnchorOffset.x, m_vAnchorOffset.y };
    transformJson["size"] = { m_vSize.x, m_vSize.y };
    transformJson["scale"] = { m_vScale.x, m_vScale.y };
    transformJson["pivot"] = { m_vPivot.x, m_vPivot.y };
    transformJson["radian"] = m_fRadian;

    data["color"] = { m_vColor.x, m_vColor.y, m_vColor.z, m_vColor.w };
    data["pass"] = m_basePass;
    data["useMask"] = m_useMask;

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
            keyframeData["scale"] = { keyframe.vScale.x, keyframe.vScale.y };
            keyframeData["angle"] = keyframe.fAngle;
            keyframeData["position"] = { keyframe.vPosition.x, keyframe.vPosition.y };
            keyframeData["color"] = { keyframe.vColor.x, keyframe.vColor.y, keyframe.vColor.z, keyframe.vColor.w };
            keyframeData["easeType"] = ENUM(keyframe.easeType);

            keyframesJson.push_back(keyframeData);
        }
        animClipsJson.push_back(clipData);
    }

    auto& childrenJson = data["children"];
    childrenJson = json::array();

    auto pContainer = Get_Component<CObjectContainer>();
    if (!pContainer) return;

    const auto& children = pContainer->Get_Children();
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
    m_InstanceName = data.value("instanceName", "");

    if (data.contains("transform"))
    {
        const auto& transformJson = data["transform"];

        m_eAnchor = static_cast<ANCHOR>(transformJson.value("anchor", 0u));

        auto anchorOffset = transformJson.value("anchorOffset", nlohmann::ordered_json::array({0.0f, 0.0f}));
        m_vAnchorOffset = {anchorOffset[0], anchorOffset[1]};

        auto size = transformJson.value("size", nlohmann::ordered_json::array({100.0f, 100.0f}));
        m_vSize = {size[0], size[1]};

        auto scale = transformJson.value("scale", nlohmann::ordered_json::array({1.0f, 1.0f}));
        m_vScale = {scale[0], scale[1]};

        auto pivot = transformJson.value("pivot", nlohmann::ordered_json::array({0.5f, 0.5f}));
        m_vPivot = {pivot[0], pivot[1]};

        m_fRadian = transformJson.value("radian", 0.0f);
    }

    auto color = data.value("color", nlohmann::ordered_json::array({1.0f, 1.0f, 1.0f, 1.0f}));
    m_vColor = {color[0], color[1], color[2], color[3]};

    m_useMask = data.value("useMask", false);

    string pass = data.value("pass", "Opaque");
    Set_BasePass(NormalizeToBasePass(pass));

    if (data.contains("animClips"))
    {
        const auto& animClipsJson = data["animClips"];
        m_AnimClips.clear();

        for (const auto& clipJson : animClipsJson)
        {
            UI_ANIM_CLIP clip = {};
            clip.strName = clipJson.value("name", "");
            clip.fDuration = clipJson.value("duration", 1.0f);
            clip.isLoop = clipJson.value("loop", false);

            if (clipJson.contains("keyframes"))
            {
                const auto& keyframesJson = clipJson["keyframes"];
                for (const auto& keyframeJson : keyframesJson)
                {
                    UI_KEYFRAME keyframe;
                    keyframe.fTime = keyframeJson.value("time", 0.0f);

                    auto vScale = keyframeJson.value("scale", nlohmann::ordered_json::array({1.0f, 1.0f}));
                    keyframe.vScale = {vScale[0], vScale[1]};

                    keyframe.fAngle = keyframeJson.value("angle", 0.0f);

                    auto vPosition = keyframeJson.value("position", nlohmann::ordered_json::array({0.0f, 0.0f}));
                    keyframe.vPosition = {vPosition[0], vPosition[1]};

                    auto vColor = keyframeJson.value("color", nlohmann::ordered_json::array({1.0f, 1.0f, 1.0f, 1.0f}));
                    keyframe.vColor = {vColor[0], vColor[1], vColor[2], vColor[3]};

                    keyframe.easeType = static_cast<EaseType>(keyframeJson.value("easeType", 0u));

                    clip.keyframes.push_back(keyframe);
                }
            }

            m_AnimClips.push_back(clip);
        }
    }

    if (data.contains("children"))
    {
        const auto& childrenJson = data["children"];
        CObjectContainer* pContainer = Get_Component<CObjectContainer>();

        for (const auto& childJson : childrenJson)
        {
            string strTypeTag = childJson.value("typeTag", "");
            if (strTypeTag.empty()) continue;

            const string& strCurrentLevelKey = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();
            CUI_Object* pChildObj = Builder::Create_UIObject({strCurrentLevelKey, "Proto_GameObject_" + strTypeTag}).Build(strTypeTag);
            if (!pChildObj) continue;

            CUIObject_Tool* pChildUI = dynamic_cast<CUIObject_Tool*>(pChildObj);
            if (pChildUI)
            {
                pChildUI->Load(childJson);
                if (pContainer) pContainer->Add_Child(pChildUI);
            }
        }
    }
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
        Set_BasePass(m_basePass);

    ImGui::TextDisabled(("BasePass : " + m_basePass).c_str());
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
    string strCombined;
    for (_int i = 0; i < m_AnimClips.size(); ++i)
        strCombined += m_AnimClips[i].strName + '\0';
    strCombined += '\0';
    
    ImGui::Combo(u8"클립", &m_iClipIndex, strCombined.c_str());
    
    // 클립 선택 없으면 리턴
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

        // 재생, 정지 
        ImGui::SeparatorText(u8"재생");
        ImGui::BeginDisabled(clip.keyframes.empty());
        if (ImGui::Button(m_isBlending ? u8"정지" : u8"재생"))
        {
            m_isBlending = !m_isBlending;
            if (m_isBlending)
                Set_Animation(m_iClipIndex, &clip.isLoop);
            else
                m_isBlending = false;
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

        ImGui::EndChild();
        ImGui::EndPopup();
    } 
}

void CUIObject_Tool::Render_GUI_Color()
{
    ImGui::SeparatorText(u8"컬러");
    ImGui::ColorEdit4(u8"컬러", reinterpret_cast<_float*>(&m_vColor));
}

void CUIObject_Tool::Render_GUI_Image(string& strTextureKey)
{
    ImGui::SeparatorText(u8"이미지");
    if (ImGui::Button(u8"선택"))
    {
        string filePath = Helper::OpenFile({{"PNG Files", "*.png"}}, "png");
        if (filePath.empty())
            return;

        string fileName = Helper::GetFileNameWithExtension(filePath);

        CGameInstance::GetInstance()->Get_ResourceMgr()->Add_ResourcePath(fileName, filePath);
        strTextureKey = fileName;

        ApplySpriteTexture(0, G_GlobalLevelKey, strTextureKey, true);

        m_vAnchorOffset = Get_AnchorOffset(m_eAnchor);
    }

    auto sprite = Get_Component<CSprite2D>();
    sprite->Render_GUI();

    const string edited = NormalizeToBasePass(sprite->Get_PassConstant());
    if (edited != m_basePass)
        Set_BasePass(edited);
}

void CUIObject_Tool::ApplySpriteTexture(_uint idx, const string& levelKey, const string& texKey, _bool applyOriginSize)
{
    auto sprite = Get_Component<CSprite2D>();
    sprite->Change_Texture(idx, levelKey, texKey);

    m_sizeMode = UISizeMode::FHD;

    auto texture = sprite->Get_Texture(idx);
    auto size    = texture->Get_Size();

    m_sizeFHD = {(float)size.x, (float)size.y};

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
     
    _uint2 vSize = Get_Component<CSprite2D>()->Get_Texture(0)->Get_Size();
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