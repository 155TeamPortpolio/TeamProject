#include "Engine_Defines.h"
#include "UI_Object.h"
#include "GameInstance.h"
#include "Model.h"
#include "Material.h"
#include "IRenderService.h"
#include "Sprite2D.h"
#include "Child.h"

CUI_Object::CUI_Object()
{
}

CUI_Object::CUI_Object(const CUI_Object& rhs)
    :CGameObject(rhs)
{
    m_WinSize = rhs.m_WinSize;
    m_vAnchorOffset = rhs.m_vAnchorOffset;
    m_vScreenOffset = rhs.m_vScreenOffset;
    m_vSize = rhs.m_vSize;
    m_vScale = rhs.m_vScale;
    m_fRadian = rhs.m_fRadian;
    m_vPivot = rhs.m_vPivot;
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

     if (pArg != nullptr) {
        m_vAnchorOffset = uiDesc->AnchorOffset;
        m_vScale = uiDesc->Scale;
        m_vSize = uiDesc->Size;
        m_fRadian = uiDesc->fRadian;

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
}

void CUI_Object::Late_Update(_float dt)
{
}

void CUI_Object::Post_EngineUpdate(_float dt)
{
    Update_UITransform();

    if (m_eRenderLayer != RENDER_LAYER::CustomOnly) {

        SPRITE_PACKET packet;
        packet.pSprite2D = Get_Component<CSprite2D>();
        packet.pWorldMatrix = m_pTransform->Get_WorldMatrix_Ptr();

        _bool isUI = (packet.pSprite2D != nullptr);
        _bool isValid = (packet.pSprite2D->IsValid());
        _bool isActive = (packet.pSprite2D->Get_CompActive());

        if(isUI && isValid && isActive)
            CGameInstance::GetInstance()->Get_RenderSystem()->Submit_UI(packet);
    }

    for (auto& child : Get_Children()) {
        if (child)
            child->Post_EngineUpdate(dt);
    }
}

_bool CUI_Object::Size_To(_fvector size, _float Speed)
{
    _vector length = XMVector2Length(size - XMLoadFloat2(&m_vSize));
    _vector nextSize;

    if (XMVectorGetX(length) < 5.0f) {
        nextSize = size;
        XMStoreFloat2(&m_vSize, nextSize);
        return true;
    }
    else {
        nextSize = XMVectorLerp(XMLoadFloat2(&m_vSize), size, Speed);
    }
    XMStoreFloat2(&m_vSize, nextSize);
    return false;
}

/*위치로 이동한다. 이건 부모가 있으면 부모 기준*/
_bool CUI_Object::Move_To(_fvector offset, _float Speed)
{
    _vector length = XMVector2Length(offset - m_vAnchorOffset);

    if (XMVectorGetX(length) < 0.2f) {
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

    ImGui::Text("WinSize: %.1f x %.1f", m_WinSize.x, m_WinSize.y);

    ImGui::SeparatorText("Layout");
    ImGui::InputFloat2("Size(px)", (float*)&m_vSize);
    ImGui::InputFloat2("Scale", (float*)&m_vScale);
    ImGui::InputFloat2("AnchorOffset(px)", (float*)&m_vAnchorOffset);

    ImGui::SliderFloat2("Pivot(0~1)", (float*)&m_vPivot, 0.f, 1.f);
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
    return { m_vScreenOffset.x - m_vPivot.x * size.x,
             m_vScreenOffset.y - m_vPivot.y * size.y };
}

void CUI_Object::Update_UITransform()
{
    _float2 sizePx = { m_vSize.x * m_vScale.x, m_vSize.y * m_vScale.y };
    m_pTransform->Scale({ sizePx.x, sizePx.y, 1.f });
    m_pTransform->Rotate({ 0.f, 0.f, m_fRadian });

    // 1) 기준 Rect(부모가 있으면 부모, 없으면 스크린)
    _float2 rectPos = { 0.f, 0.f };
    _float2 rectSize = m_WinSize;

    if (auto pChild = Get_Component<CChild>())
    {
        if (auto pParentUI = dynamic_cast<CUI_Object*>(pChild->Get_Parent()))
        {
            rectPos = pParentUI->Get_RectTopLeft_Screen();
            rectSize = pParentUI->Get_PxSize();
        }
    }

    // 2) 부모 Rect에서 앵커 지점 계산
    const _uint f = static_cast<_uint>(m_eAnchor);
    _float2 anchorPoint = rectPos;

    if (f & static_cast<_uint>(ANCHOR::Left))        anchorPoint.x = rectPos.x;
    else if (f & static_cast<_uint>(ANCHOR::Right))  anchorPoint.x = rectPos.x + rectSize.x;
    else                                             anchorPoint.x = rectPos.x + rectSize.x * 0.5f;

    if (f & static_cast<_uint>(ANCHOR::Top))         anchorPoint.y = rectPos.y;
    else if (f & static_cast<_uint>(ANCHOR::Bottom)) anchorPoint.y = rectPos.y + rectSize.y;
    else                                             anchorPoint.y = rectPos.y + rectSize.y * 0.5f;

    // 3) 내 pivot이 붙을 스크린 좌표 = anchorPoint + local offset
    m_vScreenOffset = { anchorPoint.x + m_vAnchorOffset.x,
                        anchorPoint.y + m_vAnchorOffset.y };

    // 4) pivot(0~1) 반영해서 트랜스폼 중심 위치 계산
    _float2 centerPos = {
        m_vScreenOffset.x + (0.5f - m_vPivot.x) * sizePx.x,
        m_vScreenOffset.y + (0.5f - m_vPivot.y) * sizePx.y
    };

    // 5) 스크린(좌상단 원점) -> 화면중심 원점 + Y반전
    m_pTransform->Set_Pos({
        centerPos.x - m_WinSize.x * 0.5f,
        m_WinSize.y * 0.5f - centerPos.y,
        0.f
        });
}


void CUI_Object::Rotate_Left(_float _radian)
{
    m_fRadian += _radian;
}

void CUI_Object::Align_To(ANCHOR anchor)
{
    m_eAnchor = anchor;
}

_float2 CUI_Object::Get_Point_Screen(_float2 anchor, _float x, _float y)
{
    _float2 size = Get_PxSize(); 
    _float2 TopLeft = { m_vScreenOffset.x - m_vPivot.x * size.x,m_vScreenOffset.y - m_vPivot.y * size.y };

    return { TopLeft.x + size.x * anchor.x + x, TopLeft.y + size.y * anchor.y + y };
}

_float2 CUI_Object::Get_Point_Local(_float2 anchor, _float x, _float y)
{
    _float2 size = Get_PxSize();
    _float2 TopLeft = { m_vAnchorOffset.x - m_vPivot.x * size.x,
             m_vAnchorOffset.y - m_vPivot.y * size.y };

    return { TopLeft.x + size.x * anchor.x + x, TopLeft.y + size.y * anchor.y + y };
}

void CUI_Object::Free()
{
    __super::Free();
}
