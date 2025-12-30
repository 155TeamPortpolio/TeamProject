#include "Engine_Defines.h"
#include "ClickManager.h"
#include "GameInstance.h"
#include "UI_Object.h"
#include "Sprite2D.h"

void CClickManager::Update(_float dt)
{
    if (m_ClickableObjects.empty()) return;

    auto input = CGameInstance::GetInstance()->Get_InputDev();

    POINT ptMouse{};
    GetCursorPos(&ptMouse);
    ScreenToClient(m_hWnd, &ptMouse);

    Vector2 mousePos = {(float)ptMouse.x, (float)ptMouse.y};

    m_pNewHovered = {};

    while (!m_ClickableObjects.empty())
    {
        auto obj = m_ClickableObjects.back();
        m_ClickableObjects.pop_back();

        if (!obj) continue;

        Vector2 topLeft = obj->Get_RectTopLeft_Screen();
        Vector2 sizePx = obj->Get_PxSize();

        if (mousePos.x < topLeft.x)            continue;
        if (mousePos.y < topLeft.y)            continue;
        if (mousePos.x > topLeft.x + sizePx.x) continue;
        if (mousePos.y > topLeft.y + sizePx.y) continue;

        auto sprite = obj->Get_Component<CSprite2D>();

        Vector2 local = mousePos - topLeft;

        _float u = local.x / sizePx.x;
        _float v = local.y / sizePx.y;

        if (!sprite->HitTest_AlphaUV(u, v, obj->Get_AlphaThreshold())) continue;

        m_pNewHovered = obj;

        if (input->Mouse_Tap(MOUSE_BTN::LB))
            obj->OnClick();
        break;
    }

    if (m_pHovered != m_pNewHovered)
    {
        if (m_pHovered)
            m_pHovered->Exit_Hover();

        m_pHovered = m_pNewHovered;

        if (m_pHovered)
            m_pHovered->Enter_Hover();
    }

    m_ClickableObjects.clear();
}

void CClickManager::Add_ClickableObject(CUI_Object* object)
{
	if (!object) return;

	m_ClickableObjects.push_back(object);
}

void CClickManager::Free()
{
	__super::Free();
	m_ClickableObjects.clear();
}