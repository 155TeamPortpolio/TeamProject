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

	m_ParticleCount = 35;

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
			Helper::Get_Random_Float(-3.f, 3.f),
			Helper::Get_Random_Float(-3.f, 3.f),
			Helper::Get_Random_Float(-3.f, 3.f)
		};

		m_PaperEffect[i].vVelocity = {
			Helper::Get_Random_Float(-0.5f, 0.5f),
			Helper::Get_Random_Float(-1.f, 0.1f),
			Helper::Get_Random_Float(-0.5f, 0.5f)
		};

		m_PaperEffect[i].vRotation = {
			Helper::Get_Random_Float(0.f, XM_2PI * 2),
			Helper::Get_Random_Float(0.f, XM_2PI * 2),
			Helper::Get_Random_Float(0.f, XM_2PI * 2)
		};

		m_PaperEffect[i].vAngularSpeed = {
			0.f,
			0.f,
			Helper::Get_Random_Float(-3.f, 3.f)
		};

		m_PaperEffect[i].fLifeTime.x = Helper::Get_Random_Float(5.f, 18.f);
		m_PaperEffect[i].fLifeTime.y = Helper::Get_Random_Float(0.f, m_PaperEffect[i].fLifeTime.x);

		m_PaperEffect[i].fScale = Helper::Get_Random_Float(0.1f, 0.3f);
		m_PaperEffect[i].fWindForce = Helper::Get_Random_Float(2.f, 10.f);
		m_PaperEffect[i].fGravityScale = Helper::Get_Random_Float(0.5f, 2.1f);

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
	_vector3 basePos = Get_WorldPos();
	_vector3 Dir = {
		Get_Component<CTransform>()->Get_WorldMatrix().m[2][0],
		Get_Component<CTransform>()->Get_WorldMatrix().m[2][1], 
		Get_Component<CTransform>()->Get_WorldMatrix().m[2][2], 
	};
	Dir.Normalize();

	for (size_t i = 0; i < m_ParticleCount; i++)
	{
		auto& p = m_PaperEffect[i];

		// 수명
		p.fLifeTime.y += dt;

		if (p.fLifeTime.y >= p.fLifeTime.x)
		{
			p.fLifeTime.x = Helper::Get_Random_Float(5.f, 18.f);
			p.fLifeTime.y = 0;

			m_PaperEffect[i].vPosition = {
				Helper::Get_Random_Float(-3.f, 3.f),
				Helper::Get_Random_Float(-3.f, 3.f),
				Helper::Get_Random_Float(-3.f, 3.f)
			};

			p.vVelocity = {
				Helper::Get_Random_Float(-1.f, 1.f),
				Helper::Get_Random_Float(-1.f, 0.1f),
				Helper::Get_Random_Float(-1.f, 1.f)
			};

			p.fScale = Helper::Get_Random_Float(0.1f, 0.3f);
			p.fWindForce = Helper::Get_Random_Float(2.f, 5.f);
			p.fGravityScale = Helper::Get_Random_Float(0.5f, 2.1f);
		}

		// 위아래 흔들림 계산
		float flutter = sinf(p.fLifeTime.y * 5.f) * 2.f;
		p.vVelocity.y += flutter * dt;
		
		// 중력
		p.vVelocity.y -= p.fGravityScale * dt * 0.25f;

		// 바람은 위치 직접 이동
		p.vPosition += Dir * p.fWindForce * dt;

		// Y는 velocity 기반
		p.vPosition.y += p.vVelocity.y * dt + dt;

		// 회전 누적
		p.vRotation += p.vAngularSpeed * dt;
		Matrix rot = Matrix::CreateFromYawPitchRoll(p.vRotation);

		XMVECTOR right = XMVector3TransformNormal({ 1,0,0 }, rot);
		XMVECTOR up = XMVector3TransformNormal({ 0,1,0 }, rot);
		XMVECTOR look = XMVector3TransformNormal({ 0,0,1 }, rot);

		right *= p.fScale;
		up *= p.fScale;
		look *= p.fScale;

		XMStoreFloat4(&m_InstancePaper[i].vRight, right);
		XMStoreFloat4(&m_InstancePaper[i].vUp, up);
		XMStoreFloat4(&m_InstancePaper[i].vLook, look);

		// 위치 반영
		m_InstancePaper[i].vTranslation =
		{
			basePos.x + p.vPosition.x,
			basePos.y + p.vPosition.y,
			basePos.z + p.vPosition.z,
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
