#include "pch.h"
#include "UI_GachaTextGroup.h"

#include "ObjectContainer.h"
#include "UI_GachaText.h"

void CUI_GachaTextGroup::Show(GachaGrade eGrade)
{
	Change_State(STATE::SHOW);
}

HRESULT CUI_GachaTextGroup::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CUI_GachaTextGroup::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Change_State(STATE::INVISIBLE);

	return S_OK;
}

void CUI_GachaTextGroup::Update(_float dt)
{
	if (m_eState == STATE::INVISIBLE)
		return;

	m_fTimer += dt;

	switch (m_eState)
	{
	case STATE::SHOW:
		Update_Show();
		break;
	case STATE::BLINK:
		Update_Blink(dt);
		break;
	}

	Get_Component<CObjectContainer>()->UpdateChild(dt);
}

void CUI_GachaTextGroup::Change_State(STATE eState)
{
	if (m_eState == eState)
		return;

	m_eState = eState;
	m_fTimer = 0.f;

	switch (m_eState)
	{
	case STATE::INVISIBLE:
		Set_Alive(false);
		break;
	case STATE::SHOW:
		Set_Alive(true);
		for (auto& pText : m_pTexts)
			pText->Set_Alive(true);
		break;
	case STATE::BLINK:
		m_isVisible = true;
		m_fBlinkAcc = 0.f;
		break;
	}
}

void CUI_GachaTextGroup::Update_Show()
{
	if (m_fTimer >= m_fShowDuration)
		Change_State(STATE::BLINK);
}

void CUI_GachaTextGroup::Update_Blink(_float dt)
{
	m_fBlinkAcc += dt;

	if (m_fBlinkAcc >= m_fBlinkInterval)
	{
		m_isVisible = !m_isVisible;
		m_fBlinkAcc = 0.f;

		for (auto& pText : m_pTexts)
			pText->Set_Alive(m_isVisible);
	}

	if (m_fTimer >= m_fBlinkDuration)
		Change_State(STATE::INVISIBLE); 
}