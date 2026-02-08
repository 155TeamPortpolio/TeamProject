#include "pch.h"
#include "UI_GachaText.h"

#include "StaticModel.h"
#include "Material.h"

void CUI_GachaText::Play(const string& strKey)
{
	Change_Model(strKey);
	Restart_Move();
}

void CUI_GachaText::Change_Model(const string& strKey)
{
	if (strKey.empty())
		return;

	auto pModel = Get_Component<CStaticModel>();
	auto pMaterial = Get_Component<CMaterial>();

	pModel->Link_Model("Gacha_Level", strKey + ".model");
	pMaterial->Link_Material("Gacha_Level", strKey + ".mat");
}

HRESULT CUI_GachaText::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CStaticModel>();
	Add_Component<CMaterial>();

	return S_OK;
}

HRESULT CUI_GachaText::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	auto pModel = Get_Component<CStaticModel>();
	auto pMaterial = Get_Component<CMaterial>();

	pModel->Link_Model("Gacha_Level", "GachaStage_UI_EngineB_01.model");
	pMaterial->Link_Material("Gacha_Level", "GachaStage_UI_EngineB_01.mat");

	pModel->Set_RenderType(RENDER_PASS_TYPE::RENDER_3DUI);
	pModel->ShadowCast(false);

	m_fTargetScale = m_fInitScale = m_pTransform->Get_Scale();
	m_fTargetPos = m_fInitPos = m_pTransform->Get_Pos();

	if (!pArg)
		return S_OK;

	TEXT_DESC* pDesc = static_cast<TEXT_DESC*>(pArg);

	m_fInitScale += pDesc->vScaleOffset;
	m_fInitPos += pDesc->vPosOffset;

	return S_OK;
}

void CUI_GachaText::Update(_float dt)
{
	Update_Move(dt);
}

void CUI_GachaText::Restart_Move()
{
	m_pTransform->Scale_Vector(m_fInitScale);
	m_pTransform->Set_PosVector(m_fInitPos);

	m_isMoving = true;
	m_fTimer = 0.f;
}

void CUI_GachaText::Update_Move(_float dt)
{
	if (!m_isMoving)
		return;

	m_fTimer += dt;

	_float t = m_fTimer / m_fDuration;
	t = min(t, 1.f);

	_float fEase = Math::ApplyEase(EaseType::OutExpo, t);

	_vector vScale = Vector3::Lerp(m_fInitScale, m_fTargetScale, fEase);
	m_pTransform->Scale_Vector(vScale);

	_vector vPos = Vector3::Lerp(m_fInitPos, m_fTargetPos, fEase);
	m_pTransform->Set_PosVector(vPos);

	if (t >= 1.f)
		m_isMoving = false;
}

CGameObject* CUI_GachaText::Create()
{
	CUI_GachaText* pInstance = new CUI_GachaText;
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CUI_GachaText");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_GachaText::Clone(INIT_DESC* pArg)
{
	CUI_GachaText* pInstance = new CUI_GachaText(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CUI_GachaText");
		Safe_Release(pInstance);
	}
	return pInstance;
}