#include "pch.h"
#include "KitObject.h"
#include "Material.h"
#include "StaticModel.h"

CKitObject::CKitObject()
	: CGameObject()
{
}

CKitObject::CKitObject(const CKitObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CKitObject::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	Add_Component<CStaticModel>();
	Add_Component<CMaterial>();
	return S_OK;
}

HRESULT CKitObject::Initialize(INIT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	auto* pDesc = static_cast<KIT_DESC*>(pArg);

	if (pDesc->iItemType == 0)
	{
		Get_Component<CStaticModel>()->Link_Model("Scott_Level", "InLevelItem_Prop_ResourceItem_01.model");
		Get_Component<CMaterial>()->Link_Material("Scott_Level", "InLevelItem_Prop_ResourceItem_01.mat");
	}
	else if (pDesc->iItemType == 1) {
		Get_Component<CStaticModel>()->Link_Model("Scott_Level", "InLevelItem_Prop_ResourceItem_02.model");
		Get_Component<CMaterial>()->Link_Material("Scott_Level", "InLevelItem_Prop_ResourceItem_02.mat");
	}
	else
		return E_FAIL;

	return S_OK;
}

void CKitObject::Awake()
{
}

void CKitObject::Priority_Update(_float dt)
{
}

void CKitObject::Update(_float dt)
{
	RotatePerSec(dt);
	Wave(dt);
}

void CKitObject::Late_Update(_float dt)
{
}


void CKitObject::RotatePerSec(_float dt)
{
	if (m_vDegreePerSec == _vector3::Zero)
		return;

	_quaternion curQuat = Get_Component<CTransform>()->Get_QuaternionRotate();

	_vector3 vDeltaDegree = m_vDegreePerSec * dt;
	_vector3 vDeltaRadians = {
		XMConvertToRadians(vDeltaDegree.x),
		XMConvertToRadians(vDeltaDegree.y),
		XMConvertToRadians(vDeltaDegree.z)
	};

	_quaternion deltaQuat = _quaternion::CreateFromYawPitchRoll(
		vDeltaRadians.y, vDeltaRadians.x, vDeltaRadians.z
	);

	_quaternion nextQuat = deltaQuat * curQuat;
	nextQuat.Normalize();

	Get_Component<CTransform>()->Set_Quaternion(nextQuat);
}

void CKitObject::Wave(_float dt)
{
	_float4 vLocalPos;
	XMStoreFloat4(&vLocalPos, Get_Component<CTransform>()->Get_Pos());
	
	if (m_fWaveTime == 0.f)
		m_fBaseY = vLocalPos.y;

	m_fWaveTime += dt * m_vWave.x;

	_float offset = sinf(XMConvertToRadians(m_fWaveTime)) * m_vWave.y;

	vLocalPos.y = m_fBaseY + offset;
	m_pTransform->Set_Pos(vLocalPos);
}

CKitObject* CKitObject::Create()
{
	CKitObject* pInstance = new CKitObject();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

CGameObject* CKitObject::Clone(INIT_DESC* pArg)
{
	CKitObject* pInstance = new CKitObject(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CKitObject::Free()
{
	__super::Free();
}
