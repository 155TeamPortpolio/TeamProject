#include "Light.h"
#include "GameInstance.h"
#include	"ILightService.h"
#include "GameObject.h"
CLight::CLight()
{
}

CLight::CLight(const CLight& rhs)
	: CComponent(rhs), m_Light{rhs.m_Light}
{
}

HRESULT CLight::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLight::Initialize(COMPONENT_DESC* pArg)
{
	if(pArg != nullptr){
	LIGHT_INIT_DESC* desc = static_cast<LIGHT_INIT_DESC*>(pArg);
	m_eType = desc->eType;
	m_Light.fLightRange = desc->fRange;
	m_Light.vLightAmbient = desc->vAmbient;
	m_Light.vLightDiffuse = desc->vDiffuse;
	m_Light.vLightDirection = desc->vDirection;
	m_Light.vLightSpecular = desc->vSpecular;
	}
	m_ID= CGameInstance::GetInstance()->Get_LightMgr()->Register_Light(this);
	return S_OK;
}

void CLight::Render_GUI()
{
    // 라이트 하나를 접었다 폈다 할 수 있게
    if (ImGui::TreeNode((void*)this, "Light %d", m_ID))
    {
        const char* typeNames[] = { "Directional", "Point", "Spot" };
        ImGui::Text("Type : %s", typeNames[static_cast<_int>(m_eType)]);

        ImGui::DragFloat("Range", &m_Light.fLightRange, 1.0f, 0.0f, 1000.0f, "%.1f");

        ImGui::ColorEdit3("Diffuse", &m_Light.vLightDiffuse.x);
        ImGui::ColorEdit3("Ambient", &m_Light.vLightAmbient.x);
        ImGui::ColorEdit3("Specular", &m_Light.vLightSpecular.x);

        if (m_eType == LIGHT_TYPE::DIRECTIONAL)
        {
            if (ImGui::DragFloat3("Direction", &m_Light.vLightDirection.x, 0.01f, -1.0f, 1.0f))
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
                if (ImGui::DragFloat3("Position", &pos.x, 0.1f))
                {
                    pTransform->Set_Pos(pos);
                    m_Light.vLightPosition = pos;
                }
            }
            else
            {
                ImGui::DragFloat3("Position", &m_Light.vLightPosition.x, 0.1f);
            }
        }

        ImGui::TreePop();
    }
}

void CLight::Set_Desc(const LIGHT_DESC& desc, LIGHT_TYPE eType)
{
	m_Light = desc;
	m_eType = eType;
	if(m_ID == -1)
		m_ID = CGameInstance::GetInstance()->Get_LightMgr()->Register_Light(this);
}

_float4 CLight::Get_Position()
{
	Get_Desc();
	return m_Light.vLightPosition;
}

LIGHT_DESC* CLight::Get_Desc()
{
	m_Light.vLightPosition = m_pOwner->Get_Position();
	return &m_Light;
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
		instance->m_Light =desc;
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
	//CGameInstance::GetInstance()->Get_LightMgr()->UnRegister_Light(m_ID);
}
