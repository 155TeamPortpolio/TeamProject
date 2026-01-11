#include "pch.h"
#include "AttackSign.h"
#include "GameInstance.h"
#include "PointModel.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"
#include "BoneFollower.h"

CAttackSign::CAttackSign()
	:CGameObject()
{
}

CAttackSign::CAttackSign(const CAttackSign& rhs)
	:CGameObject(rhs)
{
}

HRESULT CAttackSign::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CPointModel>();
	Add_Component<CMaterial>();
	Add_Component<CBoneFollower>();
	return S_OK;
}

HRESULT CAttackSign::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	ID3D11Device* pDevice = CGameInstance::GetInstance()->Get_Device();

	auto pModel = Get_Component<CPointModel>();
	pModel->Link_Model(G_GlobalLevelKey, "Engine_Default_Point");
	pModel->Set_RenderType(RENDER_PASS_TYPE::RENDER_EFFECT);

	auto pMaterial = Get_Component<CMaterial>();
	CMaterialInstance* pMaterialInstance = CMaterialInstance::Create_Handle("Default_Instance", "ScreenEffect", pDevice);
	pMaterialInstance->Set_Param("ScreenWidth", { &m_fScreenWidth,"float",sizeof(_float) });
	pMaterialInstance->Set_Param("ScreenHeight", { &m_fScreenHeight,"float",sizeof(_float) });
	pMaterialInstance->Set_Param("Width", { &m_fWidth,"float",sizeof(_float) });
	pMaterialInstance->Set_Param("Height", { &m_fHeight,"float",sizeof(_float) });
	pMaterialInstance->Set_Param("Color", { &m_vColor,"float3",sizeof(_float3) });
	pMaterialInstance->Set_Param("Alpha", { &m_fAlpha,"float",sizeof(_float) });
	
	CMaterialData* pMaterialData = pMaterialInstance->Get_MaterialData();
	pMaterialData->Link_Shader(G_GlobalLevelKey, "VTX_Point.hlsl");
	pMaterialData->Link_Texture(G_GlobalLevelKey, "attack_sign.png", TEXTURE_TYPE::DIFFUSE);

	pMaterial->Insert_MaterialInstance(pMaterialInstance, nullptr);

	m_isAlive = false;

	return S_OK;
}

void CAttackSign::Priority_Update(_float dt)
{
}

void CAttackSign::Update(_float dt)
{
	auto pBoneFollower = Get_Component<CBoneFollower>();
	pBoneFollower->Sync_Transform(dt, m_pTransform);

	if (m_IsActive)
	{
		if (m_fElapsedTime >= m_fDuration)
		{
			m_isAlive = false;
			m_IsActive = false;
		}
		else
		{
			m_fElapsedTime += dt;
			_float t = m_fElapsedTime / m_fDuration;
		
			m_fWidth = Math::Lerp(300.f, static_cast<_float>(g_iWinSizeX), Math::EaseOutSine(t));
			m_fHeight = Math::Lerp(40.f, 1.f, Math::EaseOutSine(t));
		}
	}
}

void CAttackSign::Late_Update(_float dt)
{
}

void CAttackSign::Active()
{
	m_isAlive = true;
	m_IsActive = true;
	m_fElapsedTime = 0.f;
}

CAttackSign* CAttackSign::Create()
{
	CAttackSign* instance = new CAttackSign();

	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to create : CAttackSign");
	}

	return instance;
}

CGameObject* CAttackSign::Clone(INIT_DESC* pArg)
{
	CAttackSign* instance = new CAttackSign(*this);

	if (FAILED(instance->Initialize(pArg)))
	{
		Safe_Release(instance);
		MSG_BOX("Failed to clone : CAttackSign");
	}

	return instance;
}

void CAttackSign::Free()
{
	__super::Free();
}
