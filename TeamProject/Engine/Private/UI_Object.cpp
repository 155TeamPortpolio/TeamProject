#include "Engine_Defines.h"
#include "UI_Object.h"
#include "GameInstance.h"
#include "Model.h"
#include "Material.h"
#include "IRenderService.h"
#include "Sprite2D.h"
#include "Child.h"
#include "ObjectContainer.h"

CUI_Object::CUI_Object(const CUI_Object& rhs) : CGameObject(rhs)
{
    m_WinSize       = rhs.m_WinSize;
    m_vAnchorOffset = rhs.m_vAnchorOffset;
    m_vScreenOffset = rhs.m_vScreenOffset;
    m_vSize         = rhs.m_vSize;
    m_vScale        = rhs.m_vScale;
    m_fRadian       = rhs.m_fRadian;
    m_vPivot        = rhs.m_vPivot;
}

HRESULT CUI_Object::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CSprite2D>();
    m_WinSize = CGameInstance::GetInstance()->Get_ClientSize();

    return S_OK;
}

HRESULT CUI_Object::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);
     UI_DESC* uiDesc = static_cast<UI_DESC*>(pArg);

     if (pArg) 
     {
        m_vAnchorOffset = uiDesc->AnchorOffset;
        m_vScale        = uiDesc->Scale;
        m_vSize         = uiDesc->Size;
        m_fRadian       = uiDesc->fRadian;
        m_eAnchor       = uiDesc->eAnchor;
        Update_UITransform();
     }
    return S_OK;
}


void CUI_Object::Pre_EngineUpdate(_float dt)
{
    __super::Pre_EngineUpdate(dt);
}

void CUI_Object::Priority_Update(_float dt)
{

}

void CUI_Object::Update(_float dt)
{
    Play_Animation(dt);
}

void CUI_Object::Late_Update(_float dt)
{

}

void CUI_Object::Post_EngineUpdate(_float dt)
{
    if (!m_isAlive) return;

    Update_UITransform();

    if (m_eRenderLayer != RENDER_LAYER::CustomOnly) {

        m_vColorLinear.x = powf(m_vColor.x, 2.2f);
        m_vColorLinear.y = powf(m_vColor.y, 2.2f);
        m_vColorLinear.z = powf(m_vColor.z, 2.2f);
        m_vColorLinear.w = m_vCombinedAlpha;        // m_vCombinedAlpha = 부모 알파 * 내 알파

        SPRITE_PACKET packet;
        packet.pSprite2D    = Get_Component<CSprite2D>();
        packet.pWorldMatrix = m_pTransform->Get_WorldMatrix_Ptr();
        packet.pColor       = &m_vColorLinear;

        _bool isUI     = (packet.pSprite2D != nullptr);
        _bool isValid  = (packet.pSprite2D->IsValid());
        _bool isActive = (packet.pSprite2D->Get_CompActive());

        if(isUI && isValid && isActive)
            CGameInstance::GetInstance()->Get_RenderSystem()->Submit_UI(packet);

        if (m_isClickable)
            CGameInstance::GetInstance()->Get_ClickMgr()->Register_ClickableObject(this);
    }

    if (CObjectContainer* pObjContainer = Get_Component<CObjectContainer>()) 
        pObjContainer->Post_EngineUpdateChild(dt);
}

_bool CUI_Object::Size_To(_fvector size, _float Speed)
{
    _vector length = XMVector2Length(size - XMLoadFloat2(&m_vSize));
    _vector nextSize;

    if (XMVectorGetX(length) < 5.0f) 
    {
        nextSize = size;
        XMStoreFloat2(&m_vSize, nextSize);
        return true;
    }
    else 
        nextSize = XMVectorLerp(XMLoadFloat2(&m_vSize), size, Speed);

    XMStoreFloat2(&m_vSize, nextSize);
    return false;
}

/*위치로 이동한다. 이건 부모가 있으면 부모 기준*/
_bool CUI_Object::Move_To(_fvector offset, _float Speed)
{
    _vector length = XMVector2Length(offset - m_vAnchorOffset);

    if (XMVectorGetX(length) < 0.2f)
    {
        XMStoreFloat2(&m_vAnchorOffset, offset);
        return true;
    }

    XMStoreFloat2(&m_vAnchorOffset, XMVectorLerp(XMLoadFloat2(&m_vAnchorOffset), offset, Speed));

    return false;
}

_bool CUI_Object::Rotate_To(_float rad, _float Speed)
{
    _vector curVec = XMVectorSetX(XMVectorZero(), m_fRadian);
    _vector targetVec = XMVectorSetX(XMVectorZero(), rad);

    _vector diffVec = XMVectorAbs(XMVectorSubtract(targetVec, curVec));

    if (XMVectorGetX(diffVec) < 0.01f)
    {
        m_fRadian = rad;
        return true;
    }

    _vector nextVec = XMVectorLerp(curVec, targetVec, Speed);
    m_fRadian = XMVectorGetX(nextVec);

    return false;
}

void CUI_Object::Render_GUI()
{
    __super::Render_GUI();

    ImGui::SeparatorText("Play Animation");
    for (_int i = 0; i < m_AnimClips.size(); ++i)
    {
        if (ImGui::Button(to_string(i).c_str()))
            Set_Animation(i);
    }

    ImGui::Text("WinSize: %.1f x %.1f", m_WinSize.x, m_WinSize.y);

    ImGui::SeparatorText("Layout");
    ImGui::InputFloat2("Size(px)", (float*)&m_vSize);
    ImGui::InputFloat2("Scale", (float*)&m_vScale);
    ImGui::InputFloat2("AnchorOffset(px)", (float*)&m_vAnchorOffset);

    _float2 tmpPivot = m_vPivot;
    if (ImGui::SliderFloat2("Pivot(0~1)", (float*)&tmpPivot, 0.f, 1.f))
        Set_Pivot(tmpPivot);

    ImGui::InputFloat2("LeftTop", (float*)&m_vLeftTop);
    ImGui::SliderAngle("Radian", &m_fRadian, -180.f, 180.f);

    ImGui::SeparatorText("Anchor");
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
}

_float2 CUI_Object::Get_RectTopLeft_Screen() 
{
    _float2 size = Get_PxSize();
    return { m_vScreenOffset.x - m_vPivot.x * size.x, m_vScreenOffset.y - m_vPivot.y * size.y };
}

void CUI_Object::Set_Pivot(_float2 newPivot)
{
    _float2 sizePx = { m_vSize.x * m_vScale.x, m_vSize.y * m_vScale.y };

    // centerPos 고정되도록 anchorOffset 보정
    m_vAnchorOffset.x += (newPivot.x - m_vPivot.x) * sizePx.x;
    m_vAnchorOffset.y += (newPivot.y - m_vPivot.y) * sizePx.y;

    m_vPivot = newPivot;
}

void CUI_Object::Update_UITransform()
{
    _float2 parentScale  = { 1.f, 1.f };
    _float  parentRadian = {};
    _float  parentAlpha  = { 1.f };

    if (auto pChild = const_cast<CUI_Object*>(this)->Get_Component<CChild>())
    {
        if (auto pParentUI = dynamic_cast<CUI_Object*>(pChild->Get_Parent()))
        {
            parentScale  = pParentUI->Get_CombinedScale();
            parentRadian = pParentUI->m_fRadian;
            parentAlpha  = pParentUI->m_vCombinedAlpha;
        }
    }

    m_vCombinedScale = parentScale * m_vScale;  // 콤바인드 스케일 = 부모 스케일 * 내 스케일
    m_vCombinedAlpha = parentAlpha * m_vColor.w;    // 콤바인드 알파 = 부모의 콤바인드 알파 * 내 알파

    _float2 sizePx = { m_vSize.x * m_vCombinedScale.x, m_vSize.y * m_vCombinedScale.y };    // 사이즈 * 콤바인드 스케일

    m_pTransform->Scale({ sizePx.x, sizePx.y, 1.f });
    m_pTransform->Rotate({ 0.f, 0.f, parentRadian + m_fRadian });

    _float2 anchorPoint = Calc_AnchorPoint();

    // pivotPos (내 pivot이 붙는 화면 좌표)
    m_vScreenOffset = { anchorPoint.x + m_vAnchorOffset.x + m_vAnimPosition.x,
                        anchorPoint.y + m_vAnchorOffset.y + m_vAnimPosition.y };

    // transform 원점이 center라고 가정한 centerPos
    _float2 centerPos = {
        m_vScreenOffset.x + (0.5f - m_vPivot.x) * sizePx.x,
        m_vScreenOffset.y + (0.5f - m_vPivot.y) * sizePx.y
    };

    m_pTransform->Set_Pos({
        centerPos.x - m_WinSize.x * 0.5f,
        m_WinSize.y * 0.5f - centerPos.y,
        0.f
        });

    // 필요하면 캐시
     m_vLeftTop = { m_vScreenOffset.x - m_vPivot.x * sizePx.x,
                   m_vScreenOffset.y - m_vPivot.y * sizePx.y };
}


_float2 CUI_Object::Calc_AnchorPoint() 
{
    _float2 rectPos  = { 0.f, 0.f };
    _float2 rectSize = m_WinSize;

    if (auto pChild = const_cast<CUI_Object*>(this)->Get_Component<CChild>())
    {
        if (auto pParentUI = dynamic_cast<CUI_Object*>(pChild->Get_Parent()))
        {
            rectPos  = pParentUI->Get_RectTopLeft_Screen();
            rectSize = pParentUI->Get_PxSize();
        }
    }

    const _uint flag = ENUM(m_eAnchor);
    _float2 anchorPoint = rectPos;

    if      (flag & ENUM(ANCHOR::Left))   anchorPoint.x = rectPos.x;
    else if (flag & ENUM(ANCHOR::Right))  anchorPoint.x = rectPos.x + rectSize.x;
    else                                  anchorPoint.x = rectPos.x + rectSize.x * 0.5f;

    if      (flag & ENUM(ANCHOR::Top))    anchorPoint.y = rectPos.y;
    else if (flag & ENUM(ANCHOR::Bottom)) anchorPoint.y = rectPos.y + rectSize.y;
    else                                  anchorPoint.y = rectPos.y + rectSize.y * 0.5f;

    return anchorPoint;
}

void CUI_Object::Play_Animation(_float dt)
{
    if (m_iCurrentClipIndex < 0 || m_iCurrentClipIndex >= (_int)m_AnimClips.size()) return;
    if (!m_isBlending) return;

    const UI_ANIM_CLIP& clip = m_AnimClips[m_iCurrentClipIndex];

    if (clip.keyframes.empty())
    {
        m_isBlending = false;
        return;
    }

    m_fBlendTime += dt;

    _float ratio = 1.f;
    if (clip.fDuration > 0.f)
        ratio = m_fBlendTime / clip.fDuration;

    ratio = clamp(ratio, 0.f, 1.f);

    const UI_KEYFRAME& firstKey = clip.keyframes.front();
    const UI_KEYFRAME& lastKey = clip.keyframes.back();

    _float firstT = 0.f;
    _float lastT = 1.f;

    if (clip.fDuration > 0.f)
    {
        firstT = firstKey.fTime / clip.fDuration;
        lastT = lastKey.fTime / clip.fDuration;
        firstT = clamp(firstT, 0.f, 1.f);
        lastT = clamp(lastT, 0.f, 1.f);
    }

    if ((_int)clip.keyframes.size() == 1 || ratio <= firstT)
    {
        m_vScale = firstKey.vScale;
        m_fRadian = XMConvertToRadians(firstKey.fAngle);
        m_vAnimPosition = firstKey.vPosition;
        m_vColor = firstKey.vColor;
    }
    else if (ratio >= lastT)
    {
        m_vScale = lastKey.vScale;
        m_fRadian = XMConvertToRadians(lastKey.fAngle);
        m_vAnimPosition = lastKey.vPosition;
        m_vColor = lastKey.vColor;
    }
    else
    {
        _int keyIdx = -1;

        for (_int i = 0; i < (_int)clip.keyframes.size() - 1; ++i)
        {
            _float t0 = clip.keyframes[i].fTime / clip.fDuration;
            _float t1 = clip.keyframes[i + 1].fTime / clip.fDuration;

            if (ratio >= t0 && ratio <= t1)
            {
                keyIdx = i;
                break;
            }
        }

        if (keyIdx >= 0)
        {
            const UI_KEYFRAME& fromKey = clip.keyframes[keyIdx];
            const UI_KEYFRAME& toKey = clip.keyframes[keyIdx + 1];

            _float fromT = fromKey.fTime / clip.fDuration;
            _float toT = toKey.fTime / clip.fDuration;

            _float localRatio = 1.f;
            if (toT != fromT)
                localRatio = (ratio - fromT) / (toT - fromT);

            localRatio = clamp(localRatio, 0.f, 1.f);

            _float easeRatio = Math::ApplyEase(fromKey.easeType, localRatio);

            m_vScale = Vector2::Lerp(fromKey.vScale, toKey.vScale, easeRatio);
            m_fRadian = XMConvertToRadians(fromKey.fAngle + (toKey.fAngle - fromKey.fAngle) * easeRatio);
            m_vAnimPosition = Vector2::Lerp(fromKey.vPosition, toKey.vPosition, easeRatio);
            m_vColor = Vector4::Lerp(fromKey.vColor, toKey.vColor, easeRatio);
        }
    }

    if (ratio >= 1.f)
    {
        if (!clip.isLoop)
        {
            m_iCurrentClipIndex = -1;
            m_isBlending = false;
        }
        else              
            m_fBlendTime = 0.f;
    }
}

void CUI_Object::Set_Animation(_uint iIndex, _bool isLoop)
{
    if (m_iCurrentClipIndex == iIndex && m_isAnimLoop == isLoop) return;

    m_iCurrentClipIndex = iIndex;
    m_isBlending = true;
    m_fBlendTime = 0.f;
}

void CUI_Object::Load(const nlohmann::ordered_json& data)
{
    if (data.empty()) return;

    m_isAlive = data.value("alive", true);

    m_InstanceName = data.value("instanceName", "");

    if (data.contains("transform"))
    {
        const auto& transformJson = data["transform"];

        m_eAnchor         = static_cast<ANCHOR>(transformJson.value("anchor", 0));
        auto anchorOffset = transformJson.value("anchorOffset", json::array({0.0f, 0.0f}));
        m_vAnchorOffset   = {anchorOffset[0], anchorOffset[1]};
        auto size         = transformJson.value("size", json::array({100.0f, 100.0f}));
        m_vSize           = {size[0], size[1]};
        auto scale        = transformJson.value("scale", json::array({1.0f, 1.0f}));
        m_vScale          = {scale[0], scale[1]};
        auto pivot        = transformJson.value("pivot", json::array({0.5f, 0.5f}));
        m_vPivot          = {pivot[0], pivot[1]};
        m_fRadian         = transformJson.value("radian", 0.0f);
    }

    auto color = data.value("color", json::array({1.0f, 1.0f, 1.0f, 1.0f}));
    m_vColor   = {color[0], color[1], color[2], color[3]};

    const string pass = data.value("pass", "Opaque");
    Get_Component<CSprite2D>()->ChangePass(pass);

    if (data.contains("animClips"))
    {
        const auto& animClipsJson = data["animClips"];
        m_AnimClips.clear();

        for (const auto& clipJson : animClipsJson)
        {
            UI_ANIM_CLIP clip{};
            clip.strName   = clipJson.value("name", "");
            clip.fDuration = clipJson.value("duration", 1.0f);
            clip.isLoop    = clipJson.value("loop", false);

            if (clipJson.contains("keyframes"))
            {
                const auto& keyframesJson = clipJson["keyframes"];
                for (const auto& keyframeJson : keyframesJson)
                {
                    UI_KEYFRAME keyframe;
                    keyframe.fTime = keyframeJson.value("time", 0.0f);

                    auto vScale     = keyframeJson.value("scale", json::array({1.0f, 1.0f}));
                    keyframe.vScale = {vScale[0], vScale[1]};

                    keyframe.fAngle = keyframeJson.value("angle", 0.0f);

                    auto vPosition     = keyframeJson.value("position", json::array({0.0f, 0.0f}));
                    keyframe.vPosition = {vPosition[0], vPosition[1]};

                    auto vColor     = keyframeJson.value("color", json::array({1.0f, 1.0f, 1.0f, 1.0f}));
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

            pChildObj->Load(childJson);

            if (pContainer)
                pContainer->Add_Child(pChildObj);
        }
    }
}

UI_HANDLE CUI_Object::Get_Handle()
{
    UI_HANDLE hObj = {};

    if (m_LevelTag.empty()) {
        hObj.Reset();
        hObj.Level = m_LevelTag;
        hObj.hObjID = m_ObjectID;
        hObj.SystemIndex = m_SystemIndex;

        return hObj;
    }

    hObj.Level = m_LevelTag;
    hObj.hObjID = m_ObjectID;
    hObj.SystemIndex = m_SystemIndex;

    return hObj;
}

UI_HANDLE CUI_Object::Get_DescendantHandle(const string& instanceName)
{
    auto pContainer = Get_Component<CObjectContainer>();
    if (!pContainer)
        return UI_HANDLE();

    auto pDescendant = dynamic_cast<CUI_Object*>(pContainer->Find_Descendant(instanceName));
    if (!pDescendant)
        return UI_HANDLE();

    return pDescendant->Get_Handle();
}

_float2 CUI_Object::Get_Point_Screen(_float2 anchor, _float x, _float y)
{
    _float2 size    = Get_PxSize(); 
    _float2 TopLeft = { m_vScreenOffset.x - m_vPivot.x * size.x, m_vScreenOffset.y - m_vPivot.y * size.y };

    return { TopLeft.x + size.x * anchor.x + x, TopLeft.y + size.y * anchor.y + y };
}

_float2 CUI_Object::Get_Point_Local(_float2 anchor, _float x, _float y)
{
    _float2 size    = Get_PxSize();
    _float2 TopLeft = { m_vAnchorOffset.x - m_vPivot.x * size.x, m_vAnchorOffset.y - m_vPivot.y * size.y };

    return { TopLeft.x + size.x * anchor.x + x, TopLeft.y + size.y * anchor.y + y };
}

void CUI_Object::Free()
{
    __super::Free();

    if (m_isClickable)
        CGameInstance::GetInstance()->Get_ClickMgr()->Unregister_ClickableObject(this);
}