#include "pch.h"
#include "UI_GachaTextGroup.h"

#include "ObjectContainer.h"
#include "UI_GachaText.h"
#include "CamDirector.h"
#include "Light.h"

void CUI_GachaTextGroup::Show(GachaGrade eGrade, const string& strCamSequenceKey)
{
	Change_State(STATE::SHOW);
	m_strCamSequence = strCamSequenceKey;
}

void CUI_GachaTextGroup::Hide()
{
	Change_State(STATE::HIDE);
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

	Change_State(STATE::HIDE);

	return S_OK;
}

void CUI_GachaTextGroup::Update(_float dt)
{
	m_fTimer += dt;

	switch (m_eState)
	{
	case STATE::SHOW:
		Update_Show();
		break;
	case STATE::BLINK:
		Update_Blink(dt);
		break;
	case STATE::HIDE:
		Update_Hide(dt);
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
	case STATE::HIDE:
		for (auto& pText : m_pTexts)
			pText->Set_Alive(false);
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
	{
		Change_State(STATE::HIDE);
	} 
}

void CUI_GachaTextGroup::Update_Hide(_float dt)
{
	m_fTimer += dt;

	if (m_fTimer < m_fCamWaitDuration)
		return;

	Set_Alive(false);
	if (m_strCamSequence.empty())
		return;

	CamDirector()->RequestSequence(m_strCamSequence);

	auto pLight = CamDirector()->GetSeqCam()->Add_Component<CLight>();
	LIGHT_DESC desc = {};
	desc.eType = LIGHT_TYPE::POINT;
	desc.vOffsetPosition = { 1.2f, 0.1f, 0.f, 0.f };
	desc.vLightDiffuse = Helper::HexToColor("#FFF3D5");
	desc.fLightRange = 100.f;
	desc.fLightIntensity = 4.f; 

	pLight->Set_Desc(desc);
}