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
    m_fLocalX = rhs.m_fLocalX;
    m_fLocalY = rhs.m_fLocalY;
    m_fSizeX = rhs.m_fSizeX;
    m_fSizeY = rhs.m_fSizeY;
    m_fRadian = rhs.m_fRadian;
    m_WinSizeX = rhs.m_WinSizeX;
    m_WinSizeY = rhs.m_WinSizeY;
    m_bAttachParent = rhs.m_bAttachParent;
}

HRESULT CUI_Object::Initialize_Prototype()
{
    __super::Initialize_Prototype();

    Add_Component<CSprite2D>();

    m_WinSizeX = CGameInstance::GetInstance()->Get_ClientSize().x;
    m_WinSizeY = CGameInstance::GetInstance()->Get_ClientSize().y;

    return S_OK;
}

HRESULT CUI_Object::Initialize(INIT_DESC* pArg)
{
    __super::Initialize(pArg);
     UI_DESC* uiDesc = static_cast<UI_DESC*>(pArg);

     if (pArg != nullptr) {
        m_fLocalX = uiDesc->fX;
        m_fLocalY = uiDesc->fY;

        m_fSizeX = uiDesc->fSizeX;
        m_fSizeY = uiDesc->fSizeY;
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

void CUI_Object::Set_Size(_fvector size)
{
    _float2 vSize = {  };
    XMStoreFloat2(&vSize, size);
    m_fSizeX = vSize.x;
    m_fSizeY = vSize.y;
}

_bool CUI_Object::Size_To(_fvector size, _float Speed)
{
    _float2 vSize = { m_fSizeX,m_fSizeY };

    _vector length = XMVector2Length(size - XMLoadFloat2(&vSize));
    _vector nextSize;

    if (XMVectorGetX(length) < 5.0f) {
        nextSize = size;
        XMStoreFloat2(&vSize, nextSize);
        m_fSizeX = vSize.x;
        m_fSizeY = vSize.y; 
        return true;
    }
    else {
        nextSize = XMVectorLerp(XMLoadFloat2(&vSize), size, Speed);
    }
    XMStoreFloat2(&vSize, nextSize);
    m_fSizeX = vSize.x;
    m_fSizeY = vSize.y;

    return false;
}

_bool CUI_Object::Move_To(_fvector pos, _float Speed)
{
    _float2 vPos = { m_fLocalX,m_fLocalY };

    _vector length = XMVector2Length(pos - XMLoadFloat2(&vPos));
    _vector nextSize;

    if (XMVectorGetX(length) < 0.2f) {
        nextSize = pos;
        return true;
    }
    else {
        nextSize = XMVectorLerp(XMLoadFloat2(&vPos), pos, Speed);
    }
    XMStoreFloat2(&vPos, nextSize);
    m_fLocalX = vPos.x;
    m_fLocalY = vPos.y;
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
    ImGui::SeparatorText("Position");
    if (ImGui::Button("Left"))  m_fLocalX -= 10.f;
    ImGui::SameLine();
    if (ImGui::Button("Right")) m_fLocalX += 10.f;
    if (ImGui::Button("Up"))			m_fLocalY -= 10.f;
    ImGui::SameLine();
    if (ImGui::Button("Down"))    m_fLocalY += 10.f;

    ImGui::SeparatorText("Scale");
    if (ImGui::Button("Sub X"))       m_fSizeX -= 10.f;
    ImGui::SameLine();
    if (ImGui::Button("Add X"))         m_fSizeX += 10.f;
    if (ImGui::Button("Sub Y"))			m_fSizeY -= 10.f;
    ImGui::SameLine();
    if (ImGui::Button("Add Y"))    m_fSizeY += 10.f;
}

void CUI_Object::Update_UITransform()
{
    m_pTransform->Scale({ m_fSizeX, m_fSizeY, 1.f });
    m_pTransform->Rotate({ 0, 0, m_fRadian });

    if (auto pChildComp = Get_Component<CChild>())
    {
        if (dynamic_cast<CUI_Object*>(pChildComp->Get_Parent())&& m_bAttachParent)
        {
            auto pParent = dynamic_cast<CUI_Object*>(pChildComp->Get_Parent());
            m_fWorldX = m_fLocalX + pParent->m_fWorldX;
            m_fWorldY = m_fLocalY + pParent->m_fWorldY;
        }
        else {
            m_fWorldX = m_fLocalX;
            m_fWorldY = m_fLocalY;
        }
    }
    else {
        m_fWorldX = m_fLocalX;
        m_fWorldY = m_fLocalY;
    }

    m_pTransform->Set_Pos({ m_fWorldX - m_WinSizeX * 0.5f, -m_fWorldY + m_WinSizeY * 0.5f, 0.f });
}

void CUI_Object::Rotate_Left(_float _radian)
{
    m_fRadian += _radian;
}

//Align_To(Anchor::Bottom | Anchor::Right, { 0,10 });
_float2 CUI_Object::Align_To(ANCHOR anchor, _float2 _pivot)
{
    _uint anchorFlags = static_cast<_uint>(anchor);

    if (anchorFlags & static_cast<_uint>(ANCHOR::Left))
        m_fLocalX = _pivot.x + HalfX();
    else if (anchorFlags & static_cast<_uint>(ANCHOR::Right))
        m_fLocalX = _pivot.x - HalfX();
    else 
        m_fLocalX = _pivot.x;

    if (anchorFlags & static_cast<_uint>(ANCHOR::Top))
        m_fLocalY = _pivot.y + HalfY();
    else if (anchorFlags & static_cast<_uint>(ANCHOR::Bottom))
        m_fLocalY = _pivot.y - HalfY();
    else 
        m_fLocalY = _pivot.y;

    return _float2(m_fLocalX, m_fLocalY);
}

void CUI_Object::Free()
{
    __super::Free();
}
