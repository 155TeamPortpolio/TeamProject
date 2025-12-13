#include "Engine_Defines.h"
#include "ParticleSystem.h"

CParticleSystem::CParticleSystem()
{
}

CParticleSystem::CParticleSystem(const CParticleSystem& rhs)
{
}

HRESULT CParticleSystem::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CParticleSystem::Initialize(COMPONENT_DESC* pArg)
{
	return E_NOTIMPL;
}

const D3D11_INPUT_ELEMENT_DESC* CParticleSystem::Get_ElementDesc(_uint DrawIndex)
{
	return nullptr;
}

const _uint CParticleSystem::Get_ElementCount(_uint DrawIndex)
{
	return _uint();
}

const string_view CParticleSystem::Get_ElementKey(_uint DrawIndex)
{
	return string_view();
}

HRESULT CParticleSystem::Draw(ID3D11DeviceContext* pContext, _uint Index)
{
	return E_NOTIMPL;
}

HRESULT CParticleSystem::Link_Model(const string& levelKey, const string& modelDataKey)
{
	return E_NOTIMPL;
}

_uint CParticleSystem::Get_MeshCount()
{
	return _uint();
}

_uint CParticleSystem::Get_MaterialIndex(_uint Index)
{
	return _uint();
}

_bool CParticleSystem::isDrawable(_uint Index)
{
	return _bool();
}

void CParticleSystem::SetDrawable(_uint Index, _bool isDraw)
{
}

MINMAX_BOX CParticleSystem::Get_LocalBoundingBox()
{
	return MINMAX_BOX();
}

MINMAX_BOX CParticleSystem::Get_WorldBoundingBox()
{
	return MINMAX_BOX();
}

vector<MINMAX_BOX> CParticleSystem::Get_MeshBoundingBoxes()
{
	return vector<MINMAX_BOX>();
}

MINMAX_BOX CParticleSystem::Get_MeshBoundingBox(_uint index)
{
	return MINMAX_BOX();
}

void CParticleSystem::Render_GUI()
{
}

CParticleSystem* CParticleSystem::Create()
{
	return nullptr;
}

CComponent* CParticleSystem::Clone()
{
	return nullptr;
}

void CParticleSystem::Free()
{
}
