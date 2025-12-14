#include "Engine_Defines.h"
#include "Builder.h"
#include "GameInstance.h"

#include "GameObjectBuilder.h"
#include "UI_Object.h"
#include "IProtoService.h"
#include "ILevelService.h"
#include "IObjectService.h"

#include "Camera.h"
#include "Model.h"
#include "Light.h"
#include "RigidBody.h"

#include "Child.h"


CGameObjectBuilder::CGameObjectBuilder(const CLONE_DESC& _cloneDesc)
	:m_pGameInstance(CGameInstance::GetInstance())
{
	if (!CGameInstance::GetInstance()->Get_LevelMgr()->Check_ValidateLevel(_cloneDesc.OriginLevel))
	{
		MSG_BOX("Origin Level Tag is Invalidate : Builder");
	}
	else {
		m_CloneDesc = _cloneDesc;
	}
	Safe_AddRef(m_pGameInstance);
}

CGameObjectBuilder::~CGameObjectBuilder()
{
	for (auto& pair : m_CompDesc)
		delete(pair.second);

	m_CompDesc.clear();

	//Safe_Delete(m_CloneDesc);
	Safe_Delete(m_layerDesc);
	Safe_Delete(m_pObjDesc);

	Safe_Release(m_pGameInstance);
}

CGameObject* CGameObjectBuilder::Build(const string& instanceKey, _uint* id)
{
	if (m_CloneDesc.OriginLevel.empty())
	{
		MSG_BOX("CLONE_DESC is missing : CGameObjectBuilder ");
		return nullptr;
	}

	//오브젝트 설명체 채우기
	if(!m_pObjDesc)
		m_pObjDesc = new GAMEOBJECT_DESC;

	m_pObjDesc->InstanceName = instanceKey;
	for (auto& pair : m_CompDesc)
		m_pObjDesc->CompDesc[pair.first] = pair.second;

	//프로토 매니저에서 가져오기
	CGameObject* instance = m_pGameInstance->Get_PrototypeMgr()->Clone_Prototype(
		m_CloneDesc.OriginLevel, m_CloneDesc.protoTag, 
		m_pObjDesc);

	if (!instance) {
		return nullptr;
	}
	/*즉 -> 클론 후에 레이어에서 삽입하고 있는 중임*/
	
	if (instance && id) {
		*id = instance->Get_ObjectID();
	}
	instance->Awake();
	return instance;
}

CGameObjectBuilder& CGameObjectBuilder::Position(const _float3 position)
{
	auto iter = m_CompDesc.find(type_index(typeid(CTransform)));

	if (iter == m_CompDesc.end())
	{
		TRANSFORM_DESC* transformDesc = new TRANSFORM_DESC();
		iter = m_CompDesc.emplace(type_index(typeid(CTransform)), transformDesc).first;
	}

	TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(iter->second);
	pDesc->vInitialPosition = position;
	return *this;
}

CGameObjectBuilder& CGameObjectBuilder::Rotate(const _float3 rotate)
{
	auto iter = m_CompDesc.find(type_index(typeid(CTransform)));


	if (iter == m_CompDesc.end())
	{
		TRANSFORM_DESC* transformDesc = new TRANSFORM_DESC();
		iter = m_CompDesc.emplace(type_index(typeid(CTransform)), transformDesc).first;
	}


	TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(iter->second);
	pDesc->vInitialEulerVector = rotate;
	return *this;
}

CGameObjectBuilder& CGameObjectBuilder::Scale(const _float3 scale)
{
	auto iter = m_CompDesc.find(type_index(typeid(CTransform)));

	if (iter == m_CompDesc.end())
	{
		TRANSFORM_DESC* transformDesc = new TRANSFORM_DESC();
		iter = m_CompDesc.emplace(type_index(typeid(CTransform)), transformDesc).first;
	}

	TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(iter->second);
	pDesc->vInitialScale = scale;
	return *this;
}

CGameObjectBuilder& CGameObjectBuilder::Collider(const COLLIDER_DESC& desc)
{
	COLLIDER_DESC* colliderDesc = new COLLIDER_DESC(desc);
	m_CompDesc.emplace(type_index(typeid(CCollider)), colliderDesc);
	return *this;
}

CGameObjectBuilder& CGameObjectBuilder::RigidBody(const RIGIDBODY_DESC& desc)
{
	RIGIDBODY_DESC* rigidbodyDesc = new RIGIDBODY_DESC(desc);
	m_CompDesc.emplace(type_index(typeid(CRigidBody)), rigidbodyDesc);
	return *this;
}

//CGameObjectBuilder& CGameObjectBuilder::AABB_Collider(const AABB_COLLIDER_DESC& desc)
//{
//	AABB_COLLIDER_DESC* AABB_Desc = new AABB_COLLIDER_DESC(desc);
//	m_CompDesc.emplace(type_index(typeid(CAABB_Collider)), AABB_Desc);
//	return *this;
//}
//
//CGameObjectBuilder& CGameObjectBuilder::OBB_Collider(const OBB_COLLIDER_DESC& desc)
//{
//	OBB_COLLIDER_DESC* OBB_Desc = new OBB_COLLIDER_DESC(desc);
//	m_CompDesc.emplace(type_index(typeid(COBB_Collider)), OBB_Desc);
//	return *this;
//}
//
//CGameObjectBuilder& CGameObjectBuilder::Sphere_Collider(const SPHERE_COLLIDER_DESC& desc)
//{
//	SPHERE_COLLIDER_DESC* SPHERE_desc = new SPHERE_COLLIDER_DESC(desc);
//	m_CompDesc.emplace(type_index(typeid(CSphere_Collider)), SPHERE_desc);
//	return *this;
//}

CGameObjectBuilder& CGameObjectBuilder::Set_Parent(const PARENT_DESC& parent)
{
	PARENT_DESC* parentDesc = new PARENT_DESC(parent);
	m_CompDesc.emplace(type_index(typeid(CChild)), parentDesc);
	return *this;
}

CGameObjectBuilder& CGameObjectBuilder::Add_ObjDesc(GAMEOBJECT_DESC* pArg)
{
	if (pArg == nullptr) return *this;
	m_pObjDesc = pArg;
	return *this;
}

CGameObjectBuilder& CGameObjectBuilder::Camera(const CAMERA_DESC& camera)
{
	CAMERA_DESC* CameraDesc = new CAMERA_DESC(camera);
	m_CompDesc.emplace(type_index(typeid(CCamera)), CameraDesc);
	return *this;
}

CGameObjectBuilder& CGameObjectBuilder::Light(const LIGHT_INIT_DESC& light)
{
	LIGHT_INIT_DESC* LightDesc = new LIGHT_INIT_DESC(light);
	m_CompDesc.emplace(type_index(typeid(CLight)), LightDesc);
	return *this;
}
