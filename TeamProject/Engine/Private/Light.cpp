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
		m_Light.fLightIntensity = desc->fIntensity;
		m_Light.fInnerCos = desc->fInnerCos;
		m_Light.fOuterCos = desc->fOuterCos;
	}

	m_ID = CGameInstance::GetInstance()->Get_LightMgr()->Register_Light(this, m_ID);
	return S_OK;
}


void CLight::NormalizeDir(_float4& dir)
{
	XMVECTOR vec = XMLoadFloat4(&dir);
	vec = XMVector3Normalize(vec);
	XMStoreFloat4(&dir, vec);
	dir.w = 0.f;
}

void CLight::GetSpotConeDegrees(const LIGHT_DESC& desc, float& innerDeg, float& outerDeg)
{
	auto ClampFloat = [](_float value, _float minValue, _float maxValue)->float
		{
			if (value < minValue) return minValue;
			if (value > maxValue) return maxValue;
			return value;
		};

	float innerCos = ClampFloat(desc.fInnerCos, -1.f, 1.f);
	float outerCos = ClampFloat(desc.fOuterCos, -1.f, 1.f);

	float innerRad = acosf(innerCos) * 2.f;
	float outerRad = acosf(outerCos) * 2.f;

	innerDeg = XMConvertToDegrees(innerRad);
	outerDeg = XMConvertToDegrees(outerRad);
}
void CLight::Render_GUI()
{
	ImGui::SeparatorText("Light");

	const float lineHeight = ImGui::GetTextLineHeightWithSpacing();
	const float childHeight = (lineHeight * 16.f) + (ImGui::GetStyle().WindowPadding.y * 2.f);

	ImGui::Checkbox("Active", &m_bActive);
	Set_CompActive(m_bActive);

	ImGui::BeginChild("##LightChild", ImVec2(0.f, childHeight), true);

	const char* typeNames[] = { "Directional", "Point", "Spot" };
	int typeIndex = static_cast<int>(m_Light.eType);

	ImGui::TextUnformatted("Type");
	if (ImGui::Combo("##LightType", &typeIndex, typeNames, IM_ARRAYSIZE(typeNames)))
	{
		m_Light.eType = static_cast<LIGHT_TYPE>(typeIndex);
		NormalizeDir(m_Light.vLightDirection);
	}

	ImGui::Separator();


	if (m_Light.eType != LIGHT_TYPE::DIRECTIONAL)
	{
		ImGui::TextUnformatted("Range");
		ImGui::DragFloat("##Range", &m_Light.fLightRange, 1.0f, 0.0f, 1000.0f, "%.1f");
	}
	else
	{
		ImGui::TextDisabled("Range (Directional: unused)");
	}

	ImGui::TextUnformatted("Intensity");
	ImGui::DragFloat("##Intensity", &m_Light.fLightIntensity, 0.1f, 0.0f, 50.0f, "%.2f");

	ImGui::TextUnformatted("Diffuse");
	ImGui::ColorEdit3("##Diffuse", &m_Light.vLightDiffuse.x);

	ImGui::TextUnformatted("Ambient");
	ImGui::ColorEdit3("##Ambient", &m_Light.vLightAmbient.x);

	ImGui::TextUnformatted("Specular");
	ImGui::ColorEdit3("##Specular", &m_Light.vLightSpecular.x);

	ImGui::Separator();

	if (m_Light.eType == LIGHT_TYPE::DIRECTIONAL)
	{
		ImGui::TextUnformatted("Direction");
		if (ImGui::DragFloat3("##Direction", &m_Light.vLightDirection.x, 0.01f, -1.0f, 1.0f))
			NormalizeDir(m_Light.vLightDirection);
	}
	else
	{
		ImGui::TextUnformatted("Offset");
		ImGui::DragFloat3("##Offset", &m_Light.vOffsetPosition.x, 0.1f);

		if (m_Light.eType == LIGHT_TYPE::SPOTLIGHT)
		{
			ImGui::Spacing();
			ImGui::TextUnformatted("Spot Cone (Degrees)");

			float innerDegUi = 0.f;
			float outerDegUi = 0.f;
			GetSpotConeDegrees(m_Light, innerDegUi, outerDegUi); // m_Light(cos) -> deg

			bool changedInner = ImGui::DragFloat("##InnerDeg", &innerDegUi, 0.1f, 0.1f, 179.0f, "Inner: %.1f");
			bool changedOuter = ImGui::DragFloat("##OuterDeg", &outerDegUi, 0.1f, 0.1f, 179.0f, "Outer: %.1f");

			if (changedInner || changedOuter)
			{
				if (innerDegUi > outerDegUi)
					std::swap(innerDegUi, outerDegUi);

				m_Light.SetSpotDegree(innerDegUi, outerDegUi); // deg -> m_Light(cos) ¹Ý¿µ
			}

			ImGui::Text("InnerCos: %.3f  OuterCos: %.3f", m_Light.fInnerCos, m_Light.fOuterCos);
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

void CLight::Set_Desc(const LIGHT_DESC& desc)
{
	m_Light = desc;
	m_Light.eType = desc.eType;

	if (m_ID == -1)
		m_ID = CGameInstance::GetInstance()->Get_LightMgr()->Register_Light(this, m_ID);
}

LIGHT_DESC CLight::SnapShot_Desc()
{
	LIGHT_DESC snapShot{};

	if (!m_pOwner) return snapShot;
	CTransform* pTransform = m_pOwner->Get_Component<CTransform>();
	if (!pTransform) return  snapShot;
	if (m_Light.eType == LIGHT_TYPE::SPOTLIGHT)
		m_Light.vLightDirection = _vector4(pTransform->Dir(STATE::LOOK));

	snapShot = m_Light;

	_vector4 lightOffset = m_Light.vOffsetPosition;
	_vector4 ownerPos = pTransform->Get_WorldPos();

	XMStoreFloat4(&snapShot.vLightPosition, lightOffset + ownerPos);

	return snapShot;
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
