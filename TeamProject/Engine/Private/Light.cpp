#include "Engine_Defines.h"
#include "Light.h"
#include "GameInstance.h"
#include	"ILightService.h"
#include "GameObject.h"
CLight::CLight()
{
}

CLight::CLight(const CLight& rhs)
	: CComponent(rhs), m_Light{ rhs.m_Light }
{
}

HRESULT CLight::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLight::Initialize(COMPONENT_DESC* pArg)
{
	if (pArg != nullptr) {
		LIGHT_INIT_DESC* desc = static_cast<LIGHT_INIT_DESC*>(pArg);
		m_Light.eType = desc->eType;
		m_Light.fLightRange = desc->fRange;
		m_Light.vLightAmbient = desc->vAmbient;
		m_Light.vLightDiffuse = desc->vDiffuse;
		m_Light.vLightDirection = desc->vDirection;
		m_Light.vLightSpecular = desc->vSpecular;
	}

	m_ID = CGameInstance::GetInstance()->Get_LightMgr()->Register_Light(this, m_ID);
	return S_OK;
}

void CLight::Render_GUI()
{
	ImGui::SeparatorText("Light");
	float childWidth = ImGui::GetContentRegionAvail().x;
	const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
	const float childHeight = (textLineHeight * 12) + (ImGui::GetStyle().WindowPadding.y * 2);

	ImGui::Checkbox("Active", &m_bActive);
	Set_CompActive(m_bActive);

	ImGui::BeginChild("##LightChild", ImVec2{ 0, childHeight }, true);
	LIGHT_TYPE eType = m_Light.eType;

	const char* typeNames[] = { "Directional", "Point", "Spot" };
	ImGui::Text("Type : %s", typeNames[static_cast<_int>(eType)]);

	ImGui::Text("Range");
	ImGui::DragFloat("##Range", &m_Light.fLightRange, 1.0f, 0.0f, 1000.0f, "%.1f");
	ImGui::Text("Intensity");
	ImGui::DragFloat("##Intensity", &m_Light.fLightIntensity, 0.5f, 0.0f, 10.0f, "%.1f");

	ImGui::Text("Diffuse");
	ImGui::ColorEdit3("Diffuse", &m_Light.vLightDiffuse.x);
	ImGui::Text("Ambient");
	ImGui::ColorEdit3("Ambient", &m_Light.vLightAmbient.x);
	ImGui::Text("Specular");
	ImGui::ColorEdit3("Specular", &m_Light.vLightSpecular.x);

	if (eType == LIGHT_TYPE::DIRECTIONAL)
	{
		ImGui::Text("Direction");
		if (ImGui::DragFloat3("##Direction", &m_Light.vLightDirection.x, 0.01f, -1.0f, 1.0f))
		{
			XMVECTOR dir = XMLoadFloat4(&m_Light.vLightDirection);
			dir = XMVector3Normalize(dir);
			XMStoreFloat4(&m_Light.vLightDirection, dir);
		}
	}
	else {

		if (auto pTransform = m_pOwner->Get_Component<CTransform>())
		{
			_float4 pos = {};
			XMStoreFloat4(&pos, pTransform->Get_Pos());
			ImGui::Text("Offset");
			if (ImGui::DragFloat3("##Offset", &pos.x, 0.1f))
			{
				pTransform->Set_Pos(pos);
				m_Light.vLightPosition = pos;
			}
		}
		else
		{
			ImGui::DragFloat3("Offset", &m_Light.vOffsetPosition.x, 0.1f);
		}
	}

	ImGui::EndChild();
}

void CLight::Set_Desc(const LIGHT_DESC& desc, LIGHT_TYPE eType)
{
	m_Light = desc;
	m_Light.eType = eType;

	if (m_ID == -1)
		m_ID = CGameInstance::GetInstance()->Get_LightMgr()->Register_Light(this, m_ID);
}

LIGHT_DESC CLight::SnapShot_Desc()
{
	LIGHT_DESC snapShot{};

	if (!m_pOwner) return snapShot;
	CTransform* pTransform = m_pOwner->Get_Component<CTransform>();
	if (!pTransform) return  snapShot;

	snapShot = m_Light;

	_vector4 lightOffset = m_Light.vOffsetPosition;
	_vector4 ownerPos = pTransform->Get_WorldPos();

	XMStoreFloat4(&snapShot.vLightPosition, lightOffset + ownerPos);

	return m_Light;
}

void CLight::Set_CompActive(_bool bActive)
{
	if (m_bActive == bActive)
		return;

	m_bActive = bActive;

	if (bActive)
		CGameInstance::GetInstance()->Get_LightMgr()->Active_Light(this, m_ID);
	else
		CGameInstance::GetInstance()->Get_LightMgr()->DeActive_Light(this, m_ID);
}

CLight* CLight::Create(const LIGHT_DESC& desc)
{
	CLight* instance = new CLight();
	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Light Comp Failed To Create : CLight");
	}
	else {
		instance->m_Light = desc;
	}
	return instance;
}

CLight* CLight::Create()
{
	CLight* instance = new CLight();
	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Light Comp Failed To Create : CLight");
	}
	return instance;
}

CComponent* CLight::Clone()
{
	return new CLight(*this);
}

void CLight::Free()
{
	__super::Free();
	CGameInstance::GetInstance()->Get_LightMgr()->UnRegister_Light(this, m_ID);
}
