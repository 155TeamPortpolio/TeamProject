#include "Engine_Defines.h"
#include "ClickManager.h"
#include "GameInstance.h"
#include "UI_Object.h"

CClickManager::CClickManager(HWND hWnd)
	: m_hWnd (hWnd)
{
}

CClickManager::~CClickManager()
{
}

void CClickManager::Update(_float dt)
{
	if (m_ClickableObjects.empty())
		return;

	POINT ptMouse = {};
	GetCursorPos(&ptMouse);
	ScreenToClient(m_hWnd, &ptMouse);

	_float2 fMouse = _float2(static_cast<_float>(ptMouse.x), static_cast<_float>(ptMouse.y));

	m_pNewHovered = nullptr;

	while (!m_ClickableObjects.empty())
	{
		auto& pObj = m_ClickableObjects.back();

		if (pObj->Is_Alive())
		{
			_float2 fCenter = pObj->Get_RectTopLeft_Screen();
			_float2 fSize = pObj->Get_PxSize();

			if (fCenter.x <= fMouse.x && fCenter.x + fSize.x >= fMouse.x &&
				fCenter.y <= fMouse.y && fCenter.y + fSize.y >= fMouse.y)
			{
				m_pNewHovered = pObj;

				if(CGameInstance::GetInstance()->Get_InputDev()->Mouse_Tap(MOUSE_BTN::LB))
					pObj->OnClick();

				break;
			}
		}

		m_ClickableObjects.pop_back();
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
	if (!object)
		return;

	m_ClickableObjects.push_back(object);
}

CClickManager* CClickManager::Create(HWND hWnd)
{
	return new CClickManager(hWnd);
}

void CClickManager::Free()
{
	__super::Free();

	m_ClickableObjects.clear();
}