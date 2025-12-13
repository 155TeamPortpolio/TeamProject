#include "Engine_Defines.h"
#include "ParticleSystem.h"
#include "VI_InstancePoint.h"
#include "GameInstance.h"
#include "ResourceMgr.h"

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

HRESULT CParticleSystem::Draw(ID3D11DeviceContext* pContext, _uint offset, _uint count)
{
	pContext->Draw(count, offset);

	return S_OK;
}

void CParticleSystem::Render_GUI()
{
}

void CParticleSystem::BuildInstanceData()
{
	m_InstanceDatas.clear();

	for (const auto& particle : m_Particles)
	{

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
}
