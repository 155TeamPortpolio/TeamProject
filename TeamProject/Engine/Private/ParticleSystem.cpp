#include "Engine_Defines.h"
#include "ParticleSystem.h"
#include "VI_InstancePoint.h"
#include "GameInstance.h"
#include "ResourceMgr.h"
#include "Helper_Func.h"

#include "Material.h"
#include "MaterialInstance.h"
#include "MaterialData.h"

#include "StructuredBuffer.h"
#include "ComputeShader.h"

/*Module*/
#include "IParticleModule.h"
#include "LifeTimeVelocity.h"
#include "LifeTimeSize.h"
#include "LifeTimeColor.h"
#include "TextureSheetAnimation.h"
#include "Noise.h"

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
	m_pTextureSheetAnimation = CTextureSheetAnimation::Create();
	m_pNoise = CNoise::Create();

	m_Modules.push_back(m_pLifeTimeVelocity);
	m_Modules.push_back(m_pLifeTimeSize);
	m_Modules.push_back(m_pLifeTimeColor);
	m_Modules.push_back(m_pTextureSheetAnimation);
	m_Modules.push_back(m_pNoise);

	Safe_AddRef(m_pLifeTimeVelocity);
	Safe_AddRef(m_pLifeTimeSize);
	Safe_AddRef(m_pLifeTimeColor);
	Safe_AddRef(m_pTextureSheetAnimation);
	Safe_AddRef(m_pNoise);

	/*---------------Compute Shader-------------*/
	ID3D11Device* pDevice = CGameInstance::GetInstance()->Get_Device();

	{
		CStructuredBuffer::DESC Desc{};

	}


	m_ComputeShaders.resize(static_cast<_uint>(SHADER::END));


	
	
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

	m_eParticleSpace = particleDesc.isWorld ? PARTICLE_SPACE::WORLD : PARTICLE_SPACE::LOCAL;
	m_fDelayDuration = particleDesc.fDelayTime;
	m_fElapsedTime = 0.f;
	m_IsLoop = particleDesc.isLoop;
	m_iBurstCount = particleDesc.iBurstCount;
	m_fSpawnPerSec = particleDesc.fSpawnPerSec;
	m_fSpawnAcc = 0.f;
	m_iSpawnParticleCount = 0;
	m_iMaxSpawnParticleCount = particleDesc.iMaxSpawnParticleCount;

	m_vStartSpeed = particleDesc.vStartSpeed;
	m_vStartLifeTime = particleDesc.vStartLifeTime;

	m_vStartSize = particleDesc.vStartSize;
	m_vSpawnAreaMin = particleDesc.vSpawnAreaMin;
	m_vSpawnAreaMax = particleDesc.vSpawnAreaMax;

	m_UseGravity = particleDesc.useGravity;
	m_fGravityScale = particleDesc.fGravityScale;

	/*Module Params*/

	/*Life Time Velocity*/
	{
		CLifeTimeVelocity::LIFE_TIME_VELOCITY_DESC Desc{};
		Desc.fDampScale = particleDesc.fDampScale;
		m_pLifeTimeVelocity->SetParams(&Desc);
	}

	/*Life Time Size*/
	{
		CLifeTimeSize::LIFE_TIME_SIZE_DESC Desc{};
		Desc.vStartScale = particleDesc.vStartScale;
		Desc.vEndScale = particleDesc.vEndScale;
		m_pLifeTimeSize->SetParams(&Desc);
	}

	/*Life Time Color*/
	{
		CLifeTimeColor::LIFE_TIME_COLOR_DESC Desc{};
		Desc.vStartColor = particleDesc.vStartColor;
		Desc.vEndColor = particleDesc.vEndColor;
		m_pLifeTimeColor->SetParams(&Desc);
	}

	/*Texture Sheet Animation*/
	{
		CTextureSheetAnimation::TEXTURE_SHEET_ANIMATION_DESC Desc{};
		Desc.isParticleAnimated = particleDesc.isParticleAnimated;
		Desc.isRandomFrameIndex = particleDesc.isRandomFrameIndex;
		Desc.iCol = particleDesc.iCol;
		Desc.iRow = particleDesc.iRow;
		Desc.iMaxFrameIndex = particleDesc.iMaxFrameIndex;
		m_pTextureSheetAnimation->SetParams(&Desc);

		m_iTextureCol = Desc.iCol;
		m_iTextureRow = Desc.iRow;

		auto customInstance = m_pOwner->Get_Component<CMaterial>()->Get_MaterialInstance(0);
		customInstance->Set_Param("Col", { &m_iTextureCol,"uint",sizeof(_uint) });
		customInstance->Set_Param("Row", { &m_iTextureRow,"uint",sizeof(_uint) });
	}

	/*Noise*/
	{
		CNoise::NOISE_DESC Desc{};
		Desc.vStrength = particleDesc.vStrength;
		Desc.vFrequency = particleDesc.vFrequency;
		Desc.vScrollSpeed = particleDesc.vScrollSpeed;
		m_pNoise->SetParams(&Desc);
	}

	m_Particles.resize(m_iMaxSpawnParticleCount);
	m_DeadParticleIndices.reserve(m_iMaxSpawnParticleCount);

	for (_uint i = 0; i < m_Particles.size(); ++i)
		m_DeadParticleIndices.push_back(i);

}

void CParticleSystem::Simulation_Particle(_float dt)
{
	m_fElapsedTime += dt;
	if (m_fElapsedTime >= m_fDelayDuration)
	{
		SpawnParticles(dt);
		UpdateParticles(dt);
		BuildInstanceData();
	}
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
			velocity.y -= m_fGravityScale * 10.f * dt;
			particle.vVelocity = velocity;
		}
		
		nextPosition = currPosition + particle.vVelocity * dt;
		particle.vPosition = nextPosition;
	}
}

void CParticleSystem::SetUpParticle(PARTICLE& particle) const
{
	particle.isAlive = true;

	if (m_eParticleSpace == PARTICLE_SPACE::WORLD)
	{
		_vector3 vWorldPos = m_pOwner->Get_Component<CTransform>()->Get_WorldPos();
		_vector3 vAreaMin = vWorldPos + m_vSpawnAreaMin;
		_vector3 vAreaMax = vWorldPos + m_vSpawnAreaMax;

		particle.vPosition.x = Helper::Get_Random_Float(vAreaMin.x, vAreaMax.x);
		particle.vPosition.y = Helper::Get_Random_Float(vAreaMin.y, vAreaMax.y);
		particle.vPosition.z = Helper::Get_Random_Float(vAreaMin.z, vAreaMax.z);

		_float fSpeed = Helper::Get_Random_Float(m_vStartSpeed.x, m_vStartSpeed.y);
		_vector3 vDir = particle.vPosition - vWorldPos;
		vDir.Normalize();

		particle.vVelocity = vDir * fSpeed;
	}
	else
	{
		particle.vPosition.x = Helper::Get_Random_Float(m_vSpawnAreaMin.x, m_vSpawnAreaMax.x);
		particle.vPosition.y = Helper::Get_Random_Float(m_vSpawnAreaMin.y, m_vSpawnAreaMax.y);
		particle.vPosition.z = Helper::Get_Random_Float(m_vSpawnAreaMin.z, m_vSpawnAreaMax.z);

		_float fSpeed = Helper::Get_Random_Float(m_vStartSpeed.x, m_vStartSpeed.y);
		_vector3 vDir = particle.vPosition - _vector3(0.f, 0.f, 0.f);
		vDir.Normalize();
	
		particle.vVelocity = vDir * fSpeed;
	}

	particle.fLifeTime = 0.f;
	particle.fMaxLifeTime = Helper::Get_Random_Float(m_vStartLifeTime.x, m_vStartLifeTime.y);

	particle.vSize = m_vStartSize;
	particle.vStartSize = particle.vSize;

	particle.vColor = _float4(1.f, 0.f, 1.f, 1.f);
	particle.fNoiseFrequency = Helper::Get_Random_Float(0.8f, 1.2f);

	if (m_pTextureSheetAnimation)
		m_pTextureSheetAnimation->SetUpParticle(particle);
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
		_vector4 right = _vector4(1.f, 0.f, 0.f, 0.f) * particle.vSize.x;
		_vector4 up = _vector4(0.f, 1.f, 0.f, 0.f) * particle.vSize.y;
		_vector4 look(0.f, 0.f, 1.f, 0.f);

		data.vRight = right;
		data.vUp = up;
		data.vLook = look;
		data.vTraslate = translate;
		data.vVelocity = velocity;
		data.vColor = particle.vColor;
		data.vLifeTime = lifeTime;
		data.iFrameIndex = particle.iFrameIndex;

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
	Safe_Release(m_pTextureSheetAnimation);
	Safe_Release(m_pNoise);
}
