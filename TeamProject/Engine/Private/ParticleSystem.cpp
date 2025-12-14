#include "Engine_Defines.h"
#include "ParticleSystem.h"
#include "VI_InstancePoint.h"
#include "GameInstance.h"
#include "ResourceMgr.h"
#include "Helper_Func.h"

/*Module*/
#include "IParticleModule.h"
#include "LifeTimeVelocity.h"
#include "LifeTimeSize.h"
#include "LifeTimeColor.h"

CParticleSystem::CParticleSystem()
{
}

CParticleSystem::CParticleSystem(const CParticleSystem& rhs)
{
}

HRESULT CParticleSystem::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticleSystem::Initialize(COMPONENT_DESC* pArg)
{
	m_pLifeTimeVelocity = CLifeTimeVelocity::Create();
	m_pLifeTimeSize = CLifeTimeSize::Create();
	m_pLifeTimeColor = CLifeTimeColor::Create();

	m_Modules.push_back(m_pLifeTimeVelocity);
	m_Modules.push_back(m_pLifeTimeSize);
	m_Modules.push_back(m_pLifeTimeColor);

	Safe_AddRef(m_pLifeTimeVelocity);
	Safe_AddRef(m_pLifeTimeSize);
	Safe_AddRef(m_pLifeTimeColor);

	return S_OK;
}

const D3D11_INPUT_ELEMENT_DESC* CParticleSystem::Get_ElementDesc(_uint DrawIndex)
{
	return m_pInstancePoint->Get_ElementDesc();
}

const _uint CParticleSystem::Get_ElementCount(_uint DrawIndex)
{
	return m_pInstancePoint->Get_ElementCount();
}

const string_view CParticleSystem::Get_ElementKey(_uint DrawIndex)
{
	return m_pInstancePoint->Get_ElementKey();
}

HRESULT CParticleSystem::Link_Model(const string& levelKey, const string& modelDataKey)
{
	Safe_Release(m_pInstancePoint);
	m_pInstancePoint = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_VIBuffer(levelKey, modelDataKey, BUFFER_TYPE::BASIC_INSTANCE_POINT);
	Safe_AddRef(m_pInstancePoint);

	return S_OK;
}

_uint CParticleSystem::Get_MeshCount()
{
	return 1;
}

_uint CParticleSystem::Get_MaterialIndex(_uint Index)
{
	return 0;
}

_bool CParticleSystem::isDrawable(_uint Index)
{
	return isDrawing;
}

void CParticleSystem::SetDrawable(_uint Index, _bool isDraw)
{
	isDrawing = isDraw;
}

MINMAX_BOX CParticleSystem::Get_LocalBoundingBox()
{
	return MINMAX_BOX{ { -0.5f, 0.f, -0.5f, }, {0.5f,0.f ,0.5f} };
}

MINMAX_BOX CParticleSystem::Get_WorldBoundingBox()
{
	MINMAX_BOX wordlBox = {};
	_float4x4* pWorldMat = m_pOwner->Get_Component<CTransform>()->Get_WorldMatrix_Ptr();
	XMStoreFloat3(&wordlBox.vMin, XMVector3TransformCoord({ -0.5f, 0.f, -0.5f }, XMLoadFloat4x4(pWorldMat)));
	XMStoreFloat3(&wordlBox.vMax, XMVector3TransformCoord({ 0.5f,0.f ,0.5f }, XMLoadFloat4x4(pWorldMat)));
	return wordlBox;
}

vector<MINMAX_BOX> CParticleSystem::Get_MeshBoundingBoxes()
{
	vector<MINMAX_BOX> boxes;
	boxes.push_back(MINMAX_BOX{ { -0.5f, -0.5f, 0.f }, {0.5f,0.5f ,0.f} });
	return boxes;
}

MINMAX_BOX CParticleSystem::Get_MeshBoundingBox(_uint index)
{
	return MINMAX_BOX{ { -0.5f, -0.5f, 0.f }, {0.5f,0.5f ,0.f} };
}

void CParticleSystem::SetParticleParams(PARTICLE_NODE particleDesc)
{
	m_Particles.clear();
	m_DeadParticleIndices.clear();

	m_IsLoop = particleDesc.isLoop;
	m_iBurstCount = particleDesc.iBurstCount;
	m_fSpawnPerSec = particleDesc.fSpawnPerSec;
	m_fSpawnAcc = 0.f;
	m_iSpawnParticleCount = 0;
	m_iMaxSpawnParticleCount = particleDesc.iMaxSpawnParticleCount;

	m_vStartSpeed = particleDesc.vStartSpeed;
	m_vStartLifeTime = particleDesc.vStartLifeTime;

	m_vStartSizeMin = particleDesc.vStartSizeMin;
	m_vStartSizeMax = particleDesc.vStartSizeMax;
	m_vSpawnAreaMin = particleDesc.vSpawnAreaMin;
	m_vSpawnAreaMax = particleDesc.vSpawnAreaMax;

	m_UseGravity = particleDesc.useGravity;
	m_fGravityScale = particleDesc.fGravityScale;

	m_Particles.resize(m_iMaxSpawnParticleCount);
	m_DeadParticleIndices.reserve(m_iMaxSpawnParticleCount);

	for (_uint i = 0; i < m_Particles.size(); ++i)
	{
		m_DeadParticleIndices.push_back(i);
	}

}

void CParticleSystem::Simulation_Particle(_float dt)
{
	SpawnParticles(dt);
	UpdateParticles(dt);
	BuildInstanceData();
}

HRESULT CParticleSystem::Draw(ID3D11DeviceContext* pContext, _uint offset, _uint count)
{
	pContext->DrawInstanced(m_pInstancePoint->Get_VertexCount(), count, 0, offset);

	return S_OK;
}

void CParticleSystem::Render_GUI()
{
}

void CParticleSystem::SpawnParticles(_float dt)
{
	if (m_iBurstCount > 0) /*Use Burst*/
	{
		_uint iBurstCount = m_iBurstCount;
		m_iBurstCount = 0;

		for (_uint i = 0; i < iBurstCount; ++i)
		{
			if (m_DeadParticleIndices.empty())
				break;

			auto& particle = m_Particles[m_DeadParticleIndices.back()];
			m_DeadParticleIndices.pop_back();

			SetUpParticle(particle);
		}
	}
	else
	{
		if (!m_IsLoop && m_iSpawnParticleCount >= m_iMaxSpawnParticleCount)
			return;

		m_fSpawnAcc += m_fSpawnPerSec * dt;
		_uint iSpawnCount = static_cast<_uint>(m_fSpawnAcc);

		if (iSpawnCount > 0)
		{
			m_fSpawnAcc -= static_cast<_float>(iSpawnCount);
			m_iSpawnParticleCount += iSpawnCount;

			for (_uint i = 0; i < iSpawnCount; ++i)
			{
				if (m_DeadParticleIndices.empty())
					break;

				auto& particle = m_Particles[m_DeadParticleIndices.back()];
				m_DeadParticleIndices.pop_back();

				SetUpParticle(particle);
			}
		}
	}
}

void CParticleSystem::UpdateParticles(_float dt)
{
	for (_uint i = 0; i < m_Particles.size(); ++i)
	{
		auto& particle = m_Particles[i];

		if (!particle.isAlive)
			continue;

		particle.fLifeTime += dt;
		if (particle.fLifeTime >= particle.fMaxLifeTime)
		{
			particle.isAlive = false;
			m_DeadParticleIndices.push_back(i);
		}

		for (const auto& module : m_Modules)
			module->Update(particle, dt);

		_vector3 currPosition = particle.vPosition;
		_vector3 nextPosition;
		if (m_UseGravity)
		{
			_vector3 velocity = particle.vVelocity;
			velocity.y -= m_fGravityScale * 10.f;
			particle.vVelocity = velocity;
		}
		
		nextPosition = currPosition + particle.vVelocity * dt;
		particle.vPosition = nextPosition;
	}
}

void CParticleSystem::SetUpParticle(PARTICLE& particle) const
{
	particle.isAlive = true;

	particle.vPosition.x = Helper::Get_Random_Float(m_vSpawnAreaMin.x, m_vSpawnAreaMax.x);
	particle.vPosition.y = Helper::Get_Random_Float(m_vSpawnAreaMin.y, m_vSpawnAreaMax.y);
	particle.vPosition.z = Helper::Get_Random_Float(m_vSpawnAreaMin.z, m_vSpawnAreaMax.z);

	particle.fLifeTime = 0.f;
	particle.fMaxLifeTime = Helper::Get_Random_Float(m_vStartLifeTime.x, m_vStartLifeTime.y);

	particle.vSize.x = Helper::Get_Random_Float(m_vStartSizeMin.x, m_vStartSizeMax.x);
	particle.vSize.y = Helper::Get_Random_Float(m_vStartSizeMin.y, m_vStartSizeMax.y);

	particle.vColor = _float4(1.f, 1.f, 1.f, 1.f);

	_float speed = Helper::Get_Random_Float(m_vStartSpeed.x, m_vStartSpeed.y);
	_vector3 dir = particle.vPosition - _vector3(0.f, 0.f, 0.f);
	dir.Normalize();
	
	particle.vVelocity = dir * speed;
}

void CParticleSystem::BuildInstanceData()
{
	m_InstanceDatas.clear();

	for (const auto& particle : m_Particles)
	{
		if (!particle.isAlive)
			continue;

		VTX_INSTANCE_POINT data{};

		_vector4 translate = _vector4(particle.vPosition.x, particle.vPosition.y, particle.vPosition.z, 1.f);
		_vector3 velocity = particle.vVelocity;
		_vector2 lifeTime(particle.fLifeTime, particle.fMaxLifeTime);
		_vector4 right(1.f, 0.f, 0.f, 0.f);
		_vector4 up(0.f, 1.f, 0.f, 0.f);
		_vector4 look(0.f, 0.f, 1.f, 0.f);

		data.vRight = right;
		data.vUp = up;
		data.vLook = look;
		data.vTraslate = translate;
		data.vVelocity = velocity;
		data.vColor = particle.vColor;
		data.vLifeTime = lifeTime;

		m_InstanceDatas.push_back(data);
	}
}

CParticleSystem* CParticleSystem::Create()
{
	CParticleSystem* instance = new CParticleSystem();

	if (FAILED(instance->Initialize_Prototype())) {
		MSG_BOX("CPointModel Create Failed : CParticleSystem");
		Safe_Release(instance);
	}

	return instance;
}

CComponent* CParticleSystem::Clone()
{
	CParticleSystem* instance = new CParticleSystem(*this);
	return instance;
}

void CParticleSystem::Free()
{
	__super::Free();
	Safe_Release(m_pInstancePoint);

	for (auto& module : m_Modules)
		Safe_Release(module);

	Safe_Release(m_pLifeTimeVelocity);
	Safe_Release(m_pLifeTimeSize);
	Safe_Release(m_pLifeTimeColor);
}
