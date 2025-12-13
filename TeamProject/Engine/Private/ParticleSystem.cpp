#include "Engine_Defines.h"
#include "ParticleSystem.h"
#include "VI_InstancePoint.h"
#include "GameInstance.h"
#include "ResourceMgr.h"
#include "Helper_Func.h"

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
	m_fSpawnPerSec = 4.f;

	m_Particles.resize(30);
	m_DeadParticleIndices.reserve(30);

	for (_uint i = 0; i < m_Particles.size(); ++i)
		m_DeadParticleIndices.push_back(i);

	m_VelocityMin = _float3(0.f, 5.f, 0.f);
	m_VelocityMax = _float3(0.f, 10.f, 0.f);
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
	m_fSpawnAcc += m_fSpawnPerSec * dt;
	_uint spawnCount = static_cast<_uint>(m_fSpawnAcc);

	if (spawnCount)
	{
		m_fSpawnAcc -= spawnCount;
		m_iSpawnParticleCount += spawnCount;

		for (_uint i = 0; i < spawnCount; ++i)
		{
			if (m_DeadParticleIndices.empty())
				break;

			auto& particle = m_Particles[m_DeadParticleIndices.back()];
			m_DeadParticleIndices.pop_back();

			SetUpParticle(particle);
		}
	}
}

void CParticleSystem::UpdateParticles(_float dt)
{
	_uint index{};
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

		_vector3 currPosition = particle.vPosition;
		_vector3 nextPosition = currPosition + particle.vVelocity * dt;

		particle.vPosition = nextPosition;
	}
}

void CParticleSystem::SetUpParticle(PARTICLE& particle) const
{
	particle.isAlive = true;
	
	particle.vPosition.x = Helper::Get_Random_Float(m_SpawnAreaMin.x, m_SpawnAreaMax.x);
	particle.vPosition.y = Helper::Get_Random_Float(m_SpawnAreaMin.y, m_SpawnAreaMax.y);
	particle.vPosition.z = Helper::Get_Random_Float(m_SpawnAreaMin.z, m_SpawnAreaMax.z);

	particle.vVelocity.x = Helper::Get_Random_Float(m_VelocityMin.x, m_VelocityMax.x);
	particle.vVelocity.y = Helper::Get_Random_Float(m_VelocityMin.y, m_VelocityMax.y);
	particle.vVelocity.z = Helper::Get_Random_Float(m_VelocityMin.z, m_VelocityMax.z);

	particle.vSize.x = Helper::Get_Random_Float(2.f, 2.f);
	particle.vSize.y = Helper::Get_Random_Float(2.f, 2.f);

	particle.fLifeTime = 0.f;
	particle.fMaxLifeTime = 2.f;
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
}
