#include "pch.h"
#include "PaperEffect.h"
#include "GameInstance.h"

#include "Texture.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"
#include "InstanceModel.h"
#include "RectModel.h"
#include "BattleSystem.h"
#include "BattlePlayer.h"

CPaperEffect::CPaperEffect()
	:CGameObject()
{
}

CPaperEffect::CPaperEffect(const CPaperEffect& rhs)
	:CGameObject(rhs)
{
}

HRESULT CPaperEffect::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Add_Component<CInstanceModel>();
	Add_Component<CMaterial>();
	return S_OK;
}

HRESULT CPaperEffect::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	m_ParticleCount = 20;

	INSTANCE_INIT_DESC instanceDesc = {};
	instanceDesc.ElementKey = "ClientPaperEff";
	instanceDesc.ElementCount = VTX_PAPERINSTANCE::iElementCount;
	instanceDesc.instanceStride = sizeof(INSTANCE_PAPER);
	instanceDesc.pElementDesc = VTX_PAPERINSTANCE::Elements;
	instanceDesc.instanceCount = m_ParticleCount;

	m_InitDesc.push_back(instanceDesc);

	CMaterialInstance* customInstance = CMaterialInstance::Create_Handle("PaperEffect", "Default", CGameInstance::GetInstance()->Get_Device());
	customInstance->ChangeTexture(TEXTURE_TYPE::DIFFUSE, 0);
	customInstance->Set_Blended(false);

	HRESULT hr = S_OK;

	Get_Component<CMaterial>()->Insert_MaterialInstance(customInstance, nullptr);
	auto MaterialDat = customInstance->Get_MaterialData();
	if (MaterialDat)
	{
		hr = MaterialDat->Link_Shader(G_GlobalLevelKey, "VTX_PaperEffect.hlsl");
		hr = MaterialDat->Link_Texture(G_GlobalLevelKey, "Eff_Particle_028.png", TEXTURE_TYPE::DIFFUSE);
	}

	if (FAILED(hr)) return hr;

	hr = Get_Component<CInstanceModel>()->Link_InstanceData(CGameInstance::GetInstance()->Get_Device(),
		m_InitDesc, G_GlobalLevelKey, "pPlane2.model");
	Get_Component<CInstanceModel>()->Link_InstanceMeshAll(0);
	Get_Component<CInstanceModel>()->ShadowCast(false);

	if (FAILED(hr)) return hr;

	for (auto& instance : Get_Component<CMaterial>()->Get_MaterialInstances())
	{
		auto tex = ResourceManager()->Load_Texture(G_GlobalLevelKey, "Eff_Particle_028.png");
		if (!tex) continue;
		instance->Set_Param("DiffuseTexture", { tex->Get_SRV(), "Texture2D", 0 });
		instance->Override_Pass("Default");
	}

	return hr;
}

void CPaperEffect::Awake()
{
	m_InstancePaper.resize(m_ParticleCount);
	m_PaperEffect.resize(m_ParticleCount);

	for (size_t i = 0; i < m_ParticleCount; i++)
	{
		m_PaperEffect[i].vPosition = {
			Helper::Get_Random_Float(-2.f, 2.f),
			Helper::Get_Random_Float(0.f, 2.f),
			Helper::Get_Random_Float(-2.f, 2.f)
		};

		m_PaperEffect[i].vVelocity = {
			Helper::Get_Random_Float(-0.5f, 0.5f),
			Helper::Get_Random_Float(1.f, 2.f),
			Helper::Get_Random_Float(-0.5f, 0.5f)
		};

		m_PaperEffect[i].vAngularSpeed = {
			0.f,
			0.f,
			Helper::Get_Random_Float(-3.f, 3.f)
		};

		m_PaperEffect[i].fLifeTime = { Helper::Get_Random_Float(2.f, 4.f), 0.f };

		m_PaperEffect[i].fScale = Helper::Get_Random_Float(0.1f, 0.3f);

		m_InstancePaper[i].vUV = {
			static_cast<_float>(Helper::Get_Random_Int(0, 1)) * 0.5f,
			static_cast<_float>(Helper::Get_Random_Int(0, 1)) * 0.5f
		};
		m_InstancePaper[i].vRight = { m_PaperEffect[i].fScale,0,0,0 };
		m_InstancePaper[i].vUp = { 0,m_PaperEffect[i].fScale,0,0 };
		m_InstancePaper[i].vLook = { 0,0,m_PaperEffect[i].fScale,0 };
		m_InstancePaper[i].vTranslation =
		{
			m_PaperEffect[i].vPosition.x,
			m_PaperEffect[i].vPosition.y,
			m_PaperEffect[i].vPosition.z,
			1.f
		};
	}
}

void CPaperEffect::Render_GUI()
{
	__super::Render_GUI();
}

void CPaperEffect::Priority_Update(_float dt)
{
}

void CPaperEffect::Update(_float dt)
{
	_float3 basePos = Get_WorldPos();

	for (size_t i = 0; i < m_ParticleCount; i++)
	{
		// 수명 증가
		m_PaperEffect[i].fLifeTime.y += dt;

		// 죽으면 리셋
		if (m_PaperEffect[i].fLifeTime.y >= m_PaperEffect[i].fLifeTime.x)
		{
			m_PaperEffect[i].fLifeTime.y = 0.f;
			m_PaperEffect[i].fLifeTime.x = Helper::Get_Random_Float(2.f, 4.f);

			m_PaperEffect[i].vPosition = {
				Helper::Get_Random_Float(-2.f, 2.f),
				Helper::Get_Random_Float(0.f, 2.f),
				Helper::Get_Random_Float(-2.f, 2.f)
			};

			m_PaperEffect[i].vVelocity = {
				Helper::Get_Random_Float(-0.5f, 0.5f),
				Helper::Get_Random_Float(1.f, 2.f),
				Helper::Get_Random_Float(-0.5f, 0.5f)
			};
		}

		// 위치 업데이트
		m_PaperEffect[i].vPosition += m_PaperEffect[i].vVelocity * dt;

		// 중력
		m_PaperEffect[i].vVelocity.y -= 3.f * dt;

		// 회전 적용 (Z축 회전 예시)
		static _float angle = 0.f;
		angle += m_PaperEffect[i].vAngularSpeed.z * dt;

		_float c = cosf(angle);
		_float s = sinf(angle);

		float fScale = m_PaperEffect[i].fScale;
		m_InstancePaper[i].vRight = { c * fScale, s * fScale, 0.f, 0.f };
		m_InstancePaper[i].vUp = { -s * fScale, c * fScale, 0.f, 0.f };

		// 위치 반영
		m_InstancePaper[i].vTranslation =
		{
			basePos.x + m_PaperEffect[i].vPosition.x,
			basePos.y + m_PaperEffect[i].vPosition.y,
			basePos.z + m_PaperEffect[i].vPosition.z,
			1.f
		};
	}

	Get_Component<CInstanceModel>()->Update_Instance(CGameInstance::GetInstance()->Get_Context(), m_InstancePaper.data(), 0, m_ParticleCount);
}

void CPaperEffect::Late_Update(_float dt)
{
}

CPaperEffect* CPaperEffect::Create()
{
	CPaperEffect* pInstance = new CPaperEffect();

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to create : CPaperEffect");
	}

	return pInstance;
}

CGameObject* CPaperEffect::Clone(INIT_DESC* pArg)
{
	CPaperEffect* pInstance = new CPaperEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to clone : CPaperEffect");
	}

	return pInstance;
}

void CPaperEffect::Free()
{
	__super::Free();
}
