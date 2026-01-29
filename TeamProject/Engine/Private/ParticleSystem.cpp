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
	/*Constant Buffer*/
	ID3D11Device* pDevice = CGameInstance::GetInstance()->Get_Device();

	{
		D3D11_BUFFER_DESC desc{};
		desc.ByteWidth = sizeof(CB_FRAME);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		pDevice->CreateBuffer(&desc, nullptr, &m_pCBFrameBuffer);
	}

	{
		D3D11_BUFFER_DESC desc{};
		desc.ByteWidth = sizeof(CB_SPAWN);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		pDevice->CreateBuffer(&desc, nullptr, &m_pCBSpawnBuffer);
	}

	{
		D3D11_BUFFER_DESC desc{};
		desc.ByteWidth = sizeof(CB_DEAD_LIST_INIT);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		pDevice->CreateBuffer(&desc, nullptr, &m_pCBDeadListInitBuffer);
	}

	{
		D3D11_BUFFER_DESC desc{};
		desc.ByteWidth = sizeof(CB_PACKED);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		pDevice->CreateBuffer(&desc, nullptr, &m_pCBPacked);
	}
	
	/* GPU Count Buffer */
	{
		D3D11_BUFFER_DESC Desc{};
		Desc.ByteWidth = sizeof(_uint);
		Desc.Usage = D3D11_USAGE_DEFAULT;
		Desc.BindFlags = 0;
		Desc.MiscFlags = 0;
		Desc.CPUAccessFlags = 0;
		Desc.StructureByteStride = 0;

		pDevice->CreateBuffer(&Desc, nullptr, &m_pCounterGPU);
	}

	/* GPU Staging Buffer */
	{
		D3D11_BUFFER_DESC Desc{};
		Desc.ByteWidth = sizeof(_uint);
		Desc.Usage = D3D11_USAGE_STAGING;
		Desc.BindFlags = 0;
		Desc.MiscFlags = 0;
		Desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		Desc.StructureByteStride = 0;

		pDevice->CreateBuffer(&Desc, nullptr, &m_pCounterStaging);
	}


	/* Set Compute Shader */
	auto resource = CGameInstance::GetInstance()->Get_ResourceMgr();
	m_ComputeShaders.resize(static_cast<_uint>(SHADER::END));

	m_ComputeShaders[ENUM(SHADER::SPAWN)] = resource->Load_ComputeShader(G_GlobalLevelKey,"CS_Particle_Spawn.hlsl");
	m_ComputeShaders[ENUM(SHADER::BASIC)] = resource->Load_ComputeShader(G_GlobalLevelKey, "CS_Particle_Basic.hlsl");
	m_ComputeShaders[ENUM(SHADER::INIT_DEAD_LIST)] = resource->Load_ComputeShader(G_GlobalLevelKey, "CS_Particle_DeadListInit.hlsl");
	m_ComputeShaders[ENUM(SHADER::BUILD)] = resource->Load_ComputeShader(G_GlobalLevelKey, "CS_Particle_BuildInstance.hlsl");

	for (const auto& shader : m_ComputeShaders)
		Safe_AddRef(shader);

	return S_OK;
}

const D3D11_INPUT_ELEMENT_DESC* CParticleSystem::Get_ElementDesc(_uint DrawIndex)
{
	return m_pPoint->Get_ElementDesc();
}

const _uint CParticleSystem::Get_ElementCount(_uint DrawIndex)
{
	return m_pPoint->Get_ElementCount();
}

const string_view CParticleSystem::Get_ElementKey(_uint DrawIndex)
{
	return m_pPoint->Get_ElementKey();
}

HRESULT CParticleSystem::Link_Model(const string& levelKey, const string& modelDataKey)
{
	Safe_Release(m_pPoint);
	m_pPoint = CGameInstance::GetInstance()->Get_ResourceMgr()->Load_VIBuffer(levelKey, modelDataKey, BUFFER_TYPE::BASIC_POINT);
	Safe_AddRef(m_pPoint);

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
	m_PendingChanged = particleDesc;
	m_IsChanged = true;
}

void CParticleSystem::Simulation_Particle(_float dt)
{
	if (m_IsChanged)
		ApplyPending();

	m_fElapsedTime += dt;
	if (m_fElapsedTime >= m_fDelayDuration)
	{
		SpawnParticles(dt);
		ResetAliveOut();
		UploadSpawnIn();
		UpdateParticles(dt);
		ReadAliveOutCount();
		swap(m_iAliveInIndex, m_iAliveOutIndex);

		BuildInstanceData();
	}
}

void CParticleSystem::Reset()
{
	m_fSpawnAcc = 0.f;
}

HRESULT CParticleSystem::Bind_Buffer(ID3D11DeviceContext* pContext)
{
	if (m_iAliveCount <= 0)
		return S_OK;

	if (m_pInstanceBuffer)
	{
		CMaterialInstance* pMaterialInstance = m_pOwner->Get_Component<CMaterial>()->Get_MaterialInstance(0);
		SHADER_PARAM param{};
		param.pData = m_pInstanceBuffer->GetSRV();
		param.iSize = sizeof(INSTANCE_DATA) * m_iMaxSpawnParticleCount;
		param.typeName = "StructuredBuffer";
		pMaterialInstance->Set_Param("InstanceDatas", param);
	}

	ID3D11Buffer* buffers[1] = { m_pPoint->Get_VertexBuffer() };
	_uint strides[1] = { m_pPoint->Get_VertexStride() };
	_uint offsets[1] = { 0 };

	pContext->IASetVertexBuffers(0, 1, buffers, strides, offsets);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	return S_OK;
}

HRESULT CParticleSystem::Draw(ID3D11DeviceContext* pContext)
{
	if (m_iAliveCount <= 0)
		return S_OK;

	pContext->DrawInstanced(m_pPoint->Get_VertexCount(), m_iAliveCount, 0, 0);

	ID3D11ShaderResourceView* pSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {
		nullptr
	};
	pContext->VSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV);

	return S_OK;
}

void CParticleSystem::Render_GUI()
{
}

void CParticleSystem::ApplyPending()
{
	if (m_PendingChanged.iMaxSpawnParticleCount <= 0)
		return;

	if (m_iMaxSpawnParticleCount != m_PendingChanged.iMaxSpawnParticleCount)
		CreateStructuredBuffers(m_PendingChanged.iMaxSpawnParticleCount);

	m_vRimLightColor = m_PendingChanged.vRimLightColor;
	m_vPivot = m_PendingChanged.vPivot;
	m_iRGBMaskMode = m_PendingChanged.iRGBMaskMode;
	m_eModuelMask = static_cast<MODULE_MASK>(m_PendingChanged.iModuleMask);
	m_eColorMode = static_cast<COLOR_MODE>(m_PendingChanged.iColorMode);
	m_eParticleSpace = m_PendingChanged.isWorld ? PARTICLE_SPACE::WORLD : PARTICLE_SPACE::LOCAL;

	m_fDelayDuration = m_PendingChanged.fDelayTime;
	m_fElapsedTime = 0.f;
	m_IsLoop = m_PendingChanged.isLoop;
	m_iBurstCount = m_PendingChanged.iBurstCount;
	m_fSpawnPerSec = m_PendingChanged.fSpawnPerSec;
	m_fSpawnAcc = 0.f;
	m_iSpawnParticleCount = 0;
	m_iMaxSpawnParticleCount = m_PendingChanged.iMaxSpawnParticleCount;

	m_vStartSpeed = m_PendingChanged.vStartSpeed;
	m_vStartLifeTime = m_PendingChanged.vStartLifeTime;

	m_vStartSize = m_PendingChanged.vStartSize;

	m_eSpawnShape = static_cast<SPAWN_SHAPE>(m_PendingChanged.SpawnShape);
	m_vCenter = m_PendingChanged.vCenter;
	m_vHalfBox = m_PendingChanged.vHalfBox;
	m_fRadius = m_PendingChanged.fRadius;

	m_UseGravity = m_PendingChanged.useGravity;
	m_fGravityScale = m_PendingChanged.fGravityScale;

	/*Module Params*/
	m_LifeTimeVelocity.fDampScale = m_PendingChanged.fDampScale;

	m_LifeTimeSize.vStartScale = m_PendingChanged.vStartScale;
	m_LifeTimeSize.vEndScale = m_PendingChanged.vEndScale;

	m_LifeTimeColor.vStartColor = m_PendingChanged.vStartColor;
	m_LifeTimeColor.vEndColor = m_PendingChanged.vEndColor;

	m_LifeTimeAlpha.vAlphaKey = m_PendingChanged.vAlphaKey;
	m_LifeTimeAlpha.vRatio = m_PendingChanged.vRatio;

	m_TextureSheetAnimation.isParticleAnimated = m_PendingChanged.isParticleAnimated;
	m_TextureSheetAnimation.isRandomFrameIndex = m_PendingChanged.isRandomFrameIndex;
	m_TextureSheetAnimation.iCol = m_PendingChanged.iCol;
	m_TextureSheetAnimation.iRow = m_PendingChanged.iRow;
	m_TextureSheetAnimation.iMaxFrameIndex = m_PendingChanged.iMaxFrameIndex;

	m_Noise.vStrength = m_PendingChanged.vStrength;
	m_Noise.vFrequency = m_PendingChanged.vFrequency;
	m_Noise.vScrollSpeed = m_PendingChanged.vScrollSpeed;

	auto customInstance = m_pOwner->Get_Component<CMaterial>()->Get_MaterialInstance(0);

	customInstance->Set_Param("RGBMask", { &m_iRGBMaskMode,"uint",sizeof(_uint) });

	customInstance->Set_Param("Col", { &m_TextureSheetAnimation.iCol,"uint",sizeof(_uint) });
	customInstance->Set_Param("Row", { &m_TextureSheetAnimation.iRow,"uint",sizeof(_uint) });

	customInstance->Set_Param("ColorMode", { &m_eColorMode,"uint",sizeof(_uint) });

	customInstance->Set_Param("Pivot", { &m_vPivot,"float2",sizeof(_float2) });

	customInstance->Set_Param("RimLightColor", { &m_vRimLightColor, "float3",sizeof(_float3) });

	m_IsChanged = false;
}

void CParticleSystem::CreateStructuredBuffers(_uint iMaxCount)
{
	ID3D11DeviceContext* pContext = CGameInstance::GetInstance()->Get_Context();

	ID3D11ShaderResourceView* pSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {
		nullptr
	};
	pContext->VSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV);

	CComputeShader::UnbindAll(pContext);
	Safe_Release(m_pInstanceBuffer);
	Safe_Release(m_pParticlesBuffer);
	Safe_Release(m_pDeadListBuffer);
	Safe_Release(m_pAliveBuffer[0]);
	Safe_Release(m_pAliveBuffer[1]);
	Safe_Release(m_pSpawnInBuffer);

	m_iAliveCount = 0;
	m_iAliveInIndex = 0;
	m_iAliveOutIndex = 1;

	/* Instance */
	{
		CStructuredBuffer::DESC desc{};
		desc.iCount = iMaxCount;
		desc.iStride = sizeof(INSTANCE_DATA);
		desc.iUAVFlag = 0;
		desc.eUsage = D3D11_USAGE_DEFAULT;
		desc.iCpuAccess = 0;

		m_pInstanceBuffer = CStructuredBuffer::Create(desc);
	}

	/* Particles */
	{
		CStructuredBuffer::DESC desc{};
		desc.iCount = iMaxCount;
		desc.iStride = sizeof(PARTICLE_GPU);
		desc.iUAVFlag = 0;
		desc.eUsage = D3D11_USAGE_DEFAULT;
		desc.iCpuAccess = 0;
		
		m_pParticlesBuffer = CStructuredBuffer::Create(desc);
	}

	/* Dead List */
	{
		CStructuredBuffer::DESC desc{};
		desc.iCount = iMaxCount;
		desc.iStride = sizeof(_uint);
		desc.iUAVFlag = D3D11_BUFFER_UAV_FLAG_APPEND;
		desc.eUsage = D3D11_USAGE_DEFAULT;
		desc.iCpuAccess = 0;
		
		m_pDeadListBuffer = CStructuredBuffer::Create(desc);

		/*초기화 컴셰 필요*/
		D3D11_MAPPED_SUBRESOURCE subResource{};
		CB_DEAD_LIST_INIT deadListInit{};
		deadListInit.iMaxParticleCount = iMaxCount;

		pContext->Map(m_pCBDeadListInitBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		memcpy_s(subResource.pData, sizeof(CB_DEAD_LIST_INIT), &deadListInit, sizeof(CB_DEAD_LIST_INIT));
		pContext->Unmap(m_pCBDeadListInitBuffer, 0);

		ID3D11UnorderedAccessView* uav = m_pDeadListBuffer->GetUAV();
		_uint initCount = 0;

		auto pComputeShader = m_ComputeShaders[ENUM(SHADER::INIT_DEAD_LIST)];
		pComputeShader->Bind(pContext);
		pComputeShader->SetCB(pContext, 2, m_pCBDeadListInitBuffer);
		pComputeShader->SetUAV(pContext, 1, m_pDeadListBuffer->GetUAV(), 0);
		pComputeShader->Dispatch1D(pContext, iMaxCount);
		CComputeShader::UnbindAll(pContext);
	}

	/* Alive */
	{
		CStructuredBuffer::DESC desc{};
		desc.iCount = iMaxCount;
		desc.iStride = sizeof(_uint);
		desc.iUAVFlag = D3D11_BUFFER_UAV_FLAG_APPEND;
		desc.eUsage = D3D11_USAGE_DEFAULT;
		desc.iCpuAccess = 0;

		for (_uint i = 0; i < 2; ++i)
			m_pAliveBuffer[i] = CStructuredBuffer::Create(desc);
	}

	/* Spawn In */
	{
		CStructuredBuffer::DESC desc{};
		desc.iCount = iMaxCount;
		desc.iStride = sizeof(PARTICLE_GPU);
		desc.UseSRV = true;
		desc.UseUAV = false;
		desc.iUAVFlag = 0;
		desc.eUsage = D3D11_USAGE_DYNAMIC;
		desc.iCpuAccess = D3D11_CPU_ACCESS_WRITE;

		m_pSpawnInBuffer = CStructuredBuffer::Create(desc);
	}
}

void CParticleSystem::ReadAliveOutCount()
{
	ID3D11DeviceContext* pDeviceContext = CGameInstance::GetInstance()->Get_Context();
	ID3D11UnorderedAccessView* pUAV = m_pAliveBuffer[m_iAliveOutIndex]->GetUAV();

	pDeviceContext->CopyStructureCount(m_pCounterGPU, 0, pUAV);
	pDeviceContext->CopyResource(m_pCounterStaging, m_pCounterGPU);

	D3D11_MAPPED_SUBRESOURCE mapSubResource{};
	if (FAILED(pDeviceContext->Map(m_pCounterStaging, 0, D3D11_MAP_READ, 0, &mapSubResource)))
	{
		m_iAliveCount = 0;
		return;
	}

	_uint iCount = *reinterpret_cast<_uint*>(mapSubResource.pData);
	pDeviceContext->Unmap(m_pCounterStaging, 0);

	m_iAliveCount = iCount;
}
void CParticleSystem::SpawnParticles(_float dt)
{
	m_SpawnList.clear();

	if (m_iBurstCount > 0) /*Use Burst*/
	{
		_uint iCapacityLeft = (m_iMaxSpawnParticleCount > m_iAliveCount)
			? (m_iMaxSpawnParticleCount - m_iAliveCount)
			: 0;

		_uint iSpawnCount = m_iBurstCount;
		iSpawnCount = min(iSpawnCount, iCapacityLeft);

		m_iBurstCount = 0;

		for (_uint i = 0; i < iSpawnCount; ++i)
		{
			PARTICLE_GPU particle{};
			SetUpParticle(particle);

			m_SpawnList.push_back(particle);
		}
	}
	else
	{
		if (!m_IsLoop && m_iSpawnParticleCount >= m_iMaxSpawnParticleCount)
			return;

		m_fSpawnAcc += m_fSpawnPerSec * dt;
		_uint iCapacityLeft = (m_iMaxSpawnParticleCount > m_iAliveCount) ? (m_iMaxSpawnParticleCount - m_iAliveCount) : 0;
		_uint iSpawnCount = static_cast<_uint>(m_fSpawnAcc);
		iSpawnCount = min(iSpawnCount, iCapacityLeft);

		if (iSpawnCount > 0)
		{
			m_fSpawnAcc = 0.f;// -= static_cast<_float>(iSpawnCount);

			if(!m_IsLoop) /*루프가 아닐때만 누적*/
				m_iSpawnParticleCount += iSpawnCount;

			for (_uint i = 0; i < iSpawnCount; ++i)
			{
				PARTICLE_GPU particle{};
				SetUpParticle(particle);

				m_SpawnList.push_back(particle);
			}
		}
	}
}

void CParticleSystem::ResetAliveOut()
{
	_uint iSpawnCount = m_SpawnList.size();

	ID3D11DeviceContext* pContext = CGameInstance::GetInstance()->Get_Context();
	ID3D11UnorderedAccessView* pUAV = m_pAliveBuffer[m_iAliveOutIndex]->GetUAV();
	_uint initialCount = 0;

	pContext->CSSetUnorderedAccessViews(0, 1, &pUAV, &initialCount);
}

void CParticleSystem::UploadSpawnIn()
{
	ID3D11DeviceContext* pContext = CGameInstance::GetInstance()->Get_Context();

	_uint iSpawnCount = m_SpawnList.size();
	if (iSpawnCount <= 0)
		return;

	auto pComputeShader = m_ComputeShaders[ENUM(SHADER::SPAWN)];

	{
		D3D11_MAPPED_SUBRESOURCE mapSubResource{};
		pContext->Map(m_pSpawnInBuffer->GetBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapSubResource);
		memcpy_s(mapSubResource.pData, m_pSpawnInBuffer->GetCount() * sizeof(PARTICLE_GPU), m_SpawnList.data(), iSpawnCount * sizeof(PARTICLE_GPU));
		pContext->Unmap(m_pSpawnInBuffer->GetBuffer(), 0);
	}

	{
		CB_SPAWN cbSpawn{};
		cbSpawn.iSpawnCount = iSpawnCount;

		D3D11_MAPPED_SUBRESOURCE mapSubResource{};
		pContext->Map(m_pCBSpawnBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapSubResource);
		memcpy_s(mapSubResource.pData, sizeof(CB_SPAWN), &cbSpawn, sizeof(CB_SPAWN));
		pContext->Unmap(m_pCBSpawnBuffer, 0);
	}

	pComputeShader->Bind(pContext);
	pComputeShader->SetCB(pContext, 1, m_pCBSpawnBuffer);
	pComputeShader->SetSRV(pContext, 1, m_pSpawnInBuffer->GetSRV());
	pComputeShader->SetUAV(pContext, 0, m_pAliveBuffer[m_iAliveOutIndex]->GetUAV());
	pComputeShader->SetUAV(pContext, 1, m_pDeadListBuffer->GetUAV());
	pComputeShader->SetUAV(pContext, 2, m_pParticlesBuffer->GetUAV());
	pComputeShader->Dispatch1D(pContext, iSpawnCount);
	CComputeShader::UnbindAll(pContext);
}

void CParticleSystem::UpdateParticles(_float dt)
{
	ID3D11DeviceContext* pContext = CGameInstance::GetInstance()->Get_Context();
	auto pComputeShader = m_ComputeShaders[ENUM(SHADER::BASIC)];

	{
		CB_FRAME cbFrame{};
		cbFrame.iModuleMask = ENUM(m_eModuelMask);
		cbFrame.fDeltaTime = dt;
		cbFrame.iAliveCount = m_iAliveCount;
		cbFrame.iMaxParticles = m_iMaxSpawnParticleCount;
		cbFrame.fGravityScale = m_fGravityScale;
		cbFrame.UseGravity = m_UseGravity ? 1 : 0;

		/* Life Time Velocity */
		cbFrame.fDampScale = m_LifeTimeVelocity.fDampScale;

		/* Life Time Size */
		cbFrame.vStartScale = m_LifeTimeSize.vStartScale;
		cbFrame.vEndScale = m_LifeTimeSize.vEndScale;

		/* Life Time Color */
		cbFrame.vStartColor = m_LifeTimeColor.vStartColor;
		cbFrame.vEndColor = m_LifeTimeColor.vEndColor;

		/* Life Time Alpha */
		cbFrame.vAlphaKey = m_LifeTimeAlpha.vAlphaKey;
		cbFrame.vRatio = m_LifeTimeAlpha.vRatio;

		/* Texture Sheet Animation */
		cbFrame.isAnimated = m_TextureSheetAnimation.isParticleAnimated ? 1 : 0;
		cbFrame.iMaxFrameIndex = m_TextureSheetAnimation.iMaxFrameIndex;

		/* Noise */
		cbFrame.fElapsedTime = m_fElapsedTime;
		cbFrame.vStrength = _float4(m_Noise.vStrength.x, m_Noise.vStrength.y, m_Noise.vStrength.z, 0.f);
		cbFrame.vFrequency = _float4(m_Noise.vFrequency.x, m_Noise.vFrequency.y, m_Noise.vFrequency.z, 0.f);
		cbFrame.vScrollSpeed = _float4(m_Noise.vScrollSpeed.x, m_Noise.vScrollSpeed.y, m_Noise.vScrollSpeed.z, 0.f);

		D3D11_MAPPED_SUBRESOURCE mapSubResource{};
		pContext->Map(m_pCBFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapSubResource);
		memcpy_s(mapSubResource.pData, sizeof(CB_FRAME), &cbFrame, sizeof(CB_FRAME));
		pContext->Unmap(m_pCBFrameBuffer, 0);
	}

	pComputeShader->Bind(pContext);
	pComputeShader->SetCB(pContext, 0, m_pCBFrameBuffer);
	pComputeShader->SetSRV(pContext, 0, m_pAliveBuffer[m_iAliveInIndex]->GetSRV());
	pComputeShader->SetUAV(pContext, 0, m_pAliveBuffer[m_iAliveOutIndex]->GetUAV());
	pComputeShader->SetUAV(pContext, 1, m_pDeadListBuffer->GetUAV());
	pComputeShader->SetUAV(pContext, 2, m_pParticlesBuffer->GetUAV());
	pComputeShader->Dispatch1D(pContext, m_iAliveCount);
	CComputeShader::UnbindAll(pContext);
}

void CParticleSystem::BuildInstanceData()
{
	ID3D11DeviceContext* pContext = CGameInstance::GetInstance()->Get_Context();

	{
		CB_PACKED cbPacked{};
		cbPacked.iInstanceCount = m_iAliveCount;

		D3D11_MAPPED_SUBRESOURCE mapSubResource{};
		pContext->Map(m_pCBPacked, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapSubResource);
		memcpy_s(mapSubResource.pData, sizeof(CB_PACKED), &cbPacked, sizeof(CB_PACKED));
		pContext->Unmap(m_pCBPacked, 0);
	}

	auto pComputeShader = m_ComputeShaders[ENUM(SHADER::BUILD)];

	pComputeShader->Bind(pContext);
	pComputeShader->SetCB(pContext, 3, m_pCBPacked);
	pComputeShader->SetSRV(pContext, 0, m_pAliveBuffer[m_iAliveInIndex]->GetSRV());
	pComputeShader->SetUAV(pContext, 2, m_pParticlesBuffer->GetUAV());
	pComputeShader->SetUAV(pContext, 3, m_pInstanceBuffer->GetUAV());
	pComputeShader->Dispatch1D(pContext, m_iAliveCount);
	CComputeShader::UnbindAll(pContext);
}

void CParticleSystem::SetUpParticle(PARTICLE_GPU& particle) const
{
	particle.IsAlive = 1;

	if (m_eParticleSpace == PARTICLE_SPACE::WORLD)
	{
		auto pTransform = m_pOwner->Get_Component<CTransform>();

		_vector3 vWorldPos = pTransform->Get_WorldPos();
		_vector3 vWorldCenter = _vector3::Transform(m_vCenter, pTransform->Get_WorldMatrix());
		_vector3 vPosition{};

		switch (m_eSpawnShape)
		{
		case Engine::CParticleSystem::SPAWN_SHAPE::SPHERE:
		{
			_float u = Helper::Get_Random_Float(0.f, 1.f);
			_float v = Helper::Get_Random_Float(0.f, 1.f);

			_float z = 1.f - 2.f * u;
			_float phi = 2.f * XM_PI * v;

			_float xy = sqrtf(max(0.f, 1.f - z * z));
			_vector3 dir{};

			dir.x = xy * cosf(phi);
			dir.y = xy * sinf(phi);
			dir.z = z;

			_float w = Helper::Get_Random_Float(0.f, 1.f);
			_float distance = m_fRadius * cbrtf(w);

			particle.vPosition = vWorldPos + dir * distance;

		}break;
		case Engine::CParticleSystem::SPAWN_SHAPE::BOX:
		{
			_vector3 vAreaMin = -1.f * m_vHalfBox;
			_vector3 vAreaMax = m_vHalfBox;

			particle.vPosition.x = Helper::Get_Random_Float(vAreaMin.x, vAreaMax.x);
			particle.vPosition.y = Helper::Get_Random_Float(vAreaMin.y, vAreaMax.y);
			particle.vPosition.z = Helper::Get_Random_Float(vAreaMin.z, vAreaMax.z);
			particle.vPosition = _vector3::Transform(particle.vPosition, pTransform->Get_WorldMatrix());
	
		}break;
		case Engine::CParticleSystem::SPAWN_SHAPE::CONE:
			break;
		default:
			break;
		}

		_float fSpeed = Helper::Get_Random_Float(m_vStartSpeed.x, m_vStartSpeed.y);
		_vector3 vDir = particle.vPosition - vWorldCenter;
		vDir.Normalize();

		particle.vVelocity = vDir * fSpeed;
	}
	else
	{
		switch (m_eSpawnShape)
		{
		case Engine::CParticleSystem::SPAWN_SHAPE::SPHERE:
		{
			_float u = Helper::Get_Random_Float(0.f, 1.f);
			_float v = Helper::Get_Random_Float(0.f, 1.f);

			_float z = 1.f - 2.f * u;
			_float phi = 2.f * XM_PI * v;

			_float xy = sqrtf(max(0.f, 1.f - z * z));
			_vector3 dir{};

			dir.x = xy * cosf(phi);
			dir.y = xy * sinf(phi);
			dir.z = z;

			_float w = Helper::Get_Random_Float(0.f, 1.f);
			_float distance = m_fRadius * cbrtf(w);

			particle.vPosition = dir * distance;
		}break;
		case Engine::CParticleSystem::SPAWN_SHAPE::BOX:
		{
			particle.vPosition.x = Helper::Get_Random_Float(-m_vHalfBox.x, m_vHalfBox.x);
			particle.vPosition.y = Helper::Get_Random_Float(-m_vHalfBox.y, m_vHalfBox.y);
			particle.vPosition.z = Helper::Get_Random_Float(-m_vHalfBox.z, m_vHalfBox.z);
		}break;
		case Engine::CParticleSystem::SPAWN_SHAPE::CONE:
			break;
		default:
			break;
		}

		_float fSpeed = Helper::Get_Random_Float(m_vStartSpeed.x, m_vStartSpeed.y);
		_vector3 vDir = particle.vPosition - m_vCenter;
		vDir.Normalize();

		particle.vVelocity = vDir * fSpeed;
	}

	particle.fLifeTime = 0.f;
	particle.fMaxLifeTime = Helper::Get_Random_Float(m_vStartLifeTime.x, m_vStartLifeTime.y);

	particle.vSize = m_vStartSize;
	particle.vStartSize = particle.vSize * m_LifeTimeSize.vStartScale;

	particle.vColor = m_LifeTimeColor.vStartColor;
	particle.vColor.w = m_LifeTimeAlpha.vAlphaKey.x;

	particle.fNoiseFrequency = Helper::Get_Random_Float(0.8f, 1.2f);

	if (m_TextureSheetAnimation.isRandomFrameIndex)
		particle.iFrameIndex = Helper::Get_Random_Int(0, m_TextureSheetAnimation.iMaxFrameIndex);
	else
		particle.iFrameIndex = 0;
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
	Safe_Release(m_pPoint);

	Safe_Release(m_pInstanceBuffer);
	Safe_Release(m_pParticlesBuffer);
	Safe_Release(m_pDeadListBuffer);
	for (_uint i = 0; i < 2; ++i)
		Safe_Release(m_pAliveBuffer[i]);
	Safe_Release(m_pSpawnInBuffer);

	for (auto& shader : m_ComputeShaders)
		Safe_Release(shader);

	Safe_Release(m_pCBDeadListInitBuffer);
	Safe_Release(m_pCBFrameBuffer);
	Safe_Release(m_pCBSpawnBuffer);
	Safe_Release(m_pCBPacked);

	Safe_Release(m_pCounterGPU);
	Safe_Release(m_pCounterStaging);
}
