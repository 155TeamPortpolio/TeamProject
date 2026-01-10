#include "Engine_Defines.h"

#include "GameObject.h"
#include "GameInstance.h"
#include "Builder.h"
#include "IRenderService.h"
#include "StaticModel.h"
#include "SkeletalModel.h"
#include "Material.h"
#include "Animator3D.h"
#include "ObjectContainer.h"
#include "Child.h"
#include "SkeletonFollower.h"
#include "IMeshProvider.h"
#include "DebugRender.h"
#include "InstanceModel.h"
#include "MaterialInstance.h"
#include "Layer.h"
#include "RigidBody.h"
#include "ParticleSystem.h"

_uint CGameObject::s_NextID = 1;

CGameObject::CGameObject()
	:m_ObjectID(s_NextID++)
{
}

CGameObject::CGameObject(const CGameObject& rhs)
	:m_ObjectID(s_NextID++), m_InstanceTag(rhs.m_InstanceTag), m_eRenderLayer{ rhs.m_eRenderLayer }
{
	/*트랜스폼은 가장 먼저.*/
	type_index transform = type_index(typeid(CTransform));

	auto iter = rhs.m_Components.find(transform);

	if (iter != rhs.m_Components.end()) {
		CComponent* myTransform = iter->second->Clone();
		myTransform->Set_Owner(this);
		m_Components.emplace(transform, myTransform);
	}


	for (auto& pair : rhs.m_Components) {

		if (pair.first == type_index(typeid(CTransform)))
			continue;

		if (pair.first == type_index(typeid(CModel)))
			continue;
		/*
		if (pair.first == type_index(typeid(CCollider)))
			continue;*/

		else {
			CComponent* comp = pair.second->Clone();
			comp->Set_Owner(this);
			m_Components.emplace(pair.first, comp);

			if (dynamic_cast<CModel*>(comp)) {
				m_Components.emplace(type_index(typeid(CModel)), comp);
				Safe_AddRef(comp);
			}

			//if (dynamic_cast<CCollider*>(comp)) {
			//	m_Components.emplace(type_index(typeid(CCollider)), comp);
			//	Safe_AddRef(comp);
			//}
		}
	}

	m_pTransform = Get_Component<CTransform>();
	Safe_AddRef(m_pTransform);
}

HRESULT CGameObject::Initialize_Prototype()
{
	//원본 생성 시, 필요 초기화들 진행
	m_pTransform = Add_Component<CTransform>();
	Safe_AddRef(m_pTransform);

	return S_OK;
}

HRESULT CGameObject::Initialize(INIT_DESC* pArg)
{
	if (!m_pTransform) {
		m_pTransform = Add_Component<CTransform>();
		Safe_AddRef(m_pTransform);
	}

	if (pArg == nullptr)
		return S_OK;

	GAMEOBJECT_DESC* obj = static_cast<GAMEOBJECT_DESC*>(pArg);

	// Transform 초기화
	auto tfIter = m_Components.find(type_index(typeid(CTransform)));
	if (tfIter != m_Components.end()) {
		auto descIter = obj->CompDesc.find(type_index(typeid(CTransform)));
		if (descIter == obj->CompDesc.end())
			tfIter->second->Initialize(nullptr);
		else
			tfIter->second->Initialize(descIter->second);
	}

	// RigidBody 초기화
	auto rbIter = m_Components.find(type_index(typeid(CRigidBody)));
	if (rbIter != m_Components.end()) {
		auto descIter = obj->CompDesc.find(type_index(typeid(CRigidBody)));
		if (descIter == obj->CompDesc.end())
			rbIter->second->Initialize(nullptr);
		else
			rbIter->second->Initialize(descIter->second);
	}

	for (auto& pair : m_Components)
	{
		if (pair.first == type_index(typeid(CTransform))) continue;
		if (pair.first == type_index(typeid(CModel))) continue;
		if (pair.first == type_index(typeid(CRigidBody))) continue;

		auto iter = obj->CompDesc.find(pair.first);
		/*각자 컴포넌트에 맞는 설명체 찾아서 넣어줌. 없으면 그냥 이니셜ㄹ라이즈*/
		if (iter == obj->CompDesc.end())
			pair.second->Initialize(nullptr);
		else
			pair.second->Initialize(iter->second);
	}

	m_InstanceName = obj->InstanceName;
	return S_OK;
}

void CGameObject::Pre_EngineUpdate(_float dt)
{
	if (CChild* pChild = Get_Component<CChild>()) {
		m_isRootObject = false;
	}
	else {
		m_isRootObject = true;
	}

	if (CObjectContainer* pObjContainer = Get_Component<CObjectContainer>()) {
		pObjContainer->Pre_EngineUpdateChild(dt);
	}
}

void CGameObject::Post_EngineUpdate(_float dt)
{
	/*패킷은 용도별로 따로 만든다.*/
	if (m_isAlive) {
		if (m_eRenderLayer != RENDER_LAYER::CustomOnly) {

			if (Get_Component<CInstanceModel>()) {
				Make_InstancePacket();
			}
			else if (Get_Component<CParticleSystem>()) {
				Make_ParticlePacket();
			}
			else {
				Make_OpaquePacket();
			}

#ifdef _DEBUG
			DEBUG_PACKET debugPacket = {};
			debugPacket.pModel = Get_Component<CModel>();
			debugPacket.pDebug = Get_Component<CDebugRender>();
			debugPacket.pWorldMatrix = m_pTransform->Get_WorldMatrix_Ptr();
			if (debugPacket.pDebug) {
				for (size_t i = 0; i < debugPacket.pDebug->Get_DebugBoxCount(); i++)
				{
					if (!debugPacket.pModel->isDrawable(i)) continue;
					debugPacket.DrawIndex = i;
					CGameInstance::GetInstance()->Get_RenderSystem()->Submit_Debug(debugPacket);
				}
			}

#endif // _DEBUG

		}
	}

	if (CObjectContainer* pObjContainer = Get_Component<CObjectContainer>()) {
		pObjContainer->Post_EngineUpdateChild(dt);
	}
}

void CGameObject::Render_GUI()
{
	m_pTransform->Render_GUI();
	for (auto& pair : m_Components) {
		if (pair.first == type_index(typeid(CTransform))) continue;
		if (pair.first == type_index(typeid(CModel))) continue;
		pair.second->Render_GUI();
	}
}

void CGameObject::RenderHierarchy(CGameObject*& SelectedObject, bool isSelected)
{
	ImGui::PushID((int)m_ObjectID);

	const vector<CGameObject*>& Children = Get_Children();

	ImGuiTreeNodeFlags flags =
		ImGuiTreeNodeFlags_Framed |
		ImGuiTreeNodeFlags_OpenOnArrow |
		//ImGuiTreeNodeFlags_SpanFullWidth |
		(isSelected ? ImGuiTreeNodeFlags_Selected : 0) |
		(Children.empty() ? (ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen) : 0);

	string TreeNodeName = m_InstanceName + " (" + to_string(Children.empty() ? 0 : Children.size()) + ")"+ "##" + to_string(m_ObjectID) ;
	bool opened = ImGui::TreeNodeEx(TreeNodeName.c_str(), flags);
	if (isSelected)
	{

		ImDrawList* draw = ImGui::GetWindowDrawList();
		ImVec2 minPos = ImGui::GetItemRectMin();
		ImVec2 maxPos = ImGui::GetItemRectMax();

		draw->AddRectFilled(
			ImVec2(minPos.x, minPos.y),
			ImVec2(minPos.x + 4.0f, maxPos.y),
			IM_COL32(237, 0, 134, 255));

		draw->AddRect(minPos, maxPos, IM_COL32(237, 0, 134, 255), 2.0f, 0, 1.0f);
	}

	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
		SelectedObject = this;

	if (opened && !Children.empty()) {
		Get_Component<CObjectContainer>()->RenderHierarchy(SelectedObject);
		ImGui::TreePop();
	}

	ImGui::PopID();

}

void CGameObject::Set_Layer(CLayer* pLayer)
{
	m_pLayer = pLayer;
}

const vector<CGameObject*> CGameObject::Get_Children()
{
	vector<CGameObject*> empty;

	CObjectContainer* pContainer = Get_Component<CObjectContainer>();
	if (pContainer) {
		return pContainer->Get_Children();
	}
	else {
		return empty;
	}
}


LAYER_DESC CGameObject::Get_LayerDesc()
{
	return LAYER_DESC{ m_LevelTag,m_pLayer->Get_LayerTag() };
}

OBJECT_HANDLE CGameObject::Get_Handle()
{
	OBJECT_HANDLE hObj = {};
	if (m_LevelTag.empty()) {
		hObj.Reset();
		return hObj;
	}

	if (!m_pLayer) {
		hObj.Reset();
		return hObj;
	}

	hObj.Layer = m_pLayer->Get_LayerTag();
	hObj.Level = m_LevelTag;
	hObj.hObjID = m_ObjectID;

	return hObj;
}

_float4x4* CGameObject::Get_WorldMatrix_Ptr()
{
	return m_pTransform->Get_WorldMatrix_Ptr();
}

_float4 CGameObject::Get_Position()
{
	_float4 pos;
	XMStoreFloat4(&pos, m_pTransform->Get_WorldPos());
	return pos;
}

Matrix CGameObject::Get_WorldMatrix()
{
	return m_pTransform->Get_WorldMatrix();
}

_vector3 CGameObject::Get_WorldPos()
{
	return m_pTransform->Get_WorldPos();
}

_quaternion CGameObject::Get_WorldQuat()
{
	_quaternion quat = m_pTransform->Get_QuaternionRotate();
	quat.Normalize();

	return quat;
}

HRESULT CGameObject::Make_OpaquePacket()
{
	OPAQUE_PACKET packet;
	packet.pModel = { nullptr };
	packet.bSkinning = false;
	packet.pMaterial = Get_Component<CMaterial>();
	packet.ObjID = m_ObjectID;
	if(!packet.pMaterial)return E_FAIL;

	packet.LookVector = m_pTransform->Dir(STATE::LOOK);
	packet.pWorldMatrix = m_pTransform->Get_WorldMatrix_Ptr();

	packet.pModel = Get_Component<CModel>();
	if (!packet.pModel || !packet.pModel->isReadyToDraw()) return E_FAIL;

	if (!packet.pModel->Get_CompActive()) return E_FAIL;

	packet.bSkinning = dynamic_cast<CSkeletalModel*>(packet.pModel);

	if (auto Animator = Get_Component<CAnimator3D>()) {
		if (Animator->Get_CompActive())
			packet.pPayLoad = Animator;
	}
	else if (auto Follower = Get_Component<CSkeletonFollower>()) {
		packet.pPayLoad = Follower;
	}
	else {
		packet.pPayLoad = monostate{};
	}

	if (packet.pModel == nullptr) {
		return E_FAIL;
	}

	for (size_t i = 0; i < packet.pModel->Get_MeshCount(); i++)
	{
		if (!packet.pModel->isDrawable(i)) continue;
		packet.DrawIndex = i;
		packet.MaterialIndex = packet.pModel->Get_MaterialIndex(i);

		if (packet.pMaterial->Get_MaterialInstance(packet.MaterialIndex)->IsBlened()) {
			Make_BlendedPacket(packet);
		}
		else if (packet.pModel->Get_RenderType() == RENDER_PASS_TYPE::RENDER_OPAQUE)
		{
			if(packet.bSkinning) CGameInstance::GetInstance()->Get_RenderSystem()->Submit_SkinnedMesh_Opaque(packet);
			else  CGameInstance::GetInstance()->Get_RenderSystem()->Submit_StaticMesh_Opaque(packet);
		}
		else if (packet.pModel->Get_RenderType() == RENDER_PASS_TYPE::PRIORITY)
			CGameInstance::GetInstance()->Get_RenderSystem()->Submit_Priority(packet);
		else if (packet.pModel->Get_RenderType() == RENDER_PASS_TYPE::NONLIGHT_OPAQUE)
			CGameInstance::GetInstance()->Get_RenderSystem()->Submit_NonLight(packet);
		else if (packet.pModel->Get_RenderType() == RENDER_PASS_TYPE::RENDER_EFFECT)
			Make_EffectPacket(packet);
		else if (packet.pModel->Get_RenderType() == RENDER_PASS_TYPE::RENDER_3DUI)
			Make_3DUIPacket(packet);

		 if (packet.pModel->doShadowCast()) {
			 if (packet.pMaterial->isValid(packet.MaterialIndex))
			 {
				 if (packet.bSkinning)
					 CGameInstance::GetInstance()->Get_RenderSystem()->Submit_SkinnedShadow(packet);
				 else
					 CGameInstance::GetInstance()->Get_RenderSystem()->Submit_StaticShadow(packet);

			 }
		 }
	}
	return S_OK;
}

HRESULT CGameObject::Make_BlendedPacket(OPAQUE_PACKET packet)
{
	BLENDED_PACKET newPacket = {};
	newPacket.bSkinning = packet.bSkinning;
	newPacket.DrawIndex = packet.DrawIndex;
	newPacket.MaterialIndex = packet.MaterialIndex;
	newPacket.SkinningOffset = packet.SkinningOffset;
	newPacket.pModel = packet.pModel;
	newPacket.pMaterial = packet.pMaterial;
	newPacket.pPayLoad = packet.pPayLoad;
	newPacket.pWorldMatrix = packet.pWorldMatrix;
	newPacket.ObjID = m_ObjectID;
	//float3 toObj = objWorldPos - cameraPos;
	//float dist = dot(toObj, cameraForward);

	const _float4x4* viewInverseMat = CGameInstance::GetInstance()->Get_CameraMgr()->Get_InversedViewMatrix();
	_matrix viewInverse = XMLoadFloat4x4(viewInverseMat);

	_float4 camPos = CGameInstance::GetInstance()->Get_CameraMgr()->Get_CameraPos();

	_vector CamDist = m_pTransform->Get_Pos() - XMLoadFloat4(&camPos);
	_vector CamForward = XMVector4Normalize(viewInverse.r[2]);

	newPacket.DistanceToCamera = XMVectorGetX(XMVector4Dot(CamDist, CamForward));

	CGameInstance::GetInstance()->Get_RenderSystem()->Submit_Blend(newPacket);
	return S_OK;
}

HRESULT CGameObject::Make_NonLightPacket(OPAQUE_PACKET packet)
{
	CGameInstance::GetInstance()->Get_RenderSystem()->Submit_NonLight(packet);
	return S_OK;
}

HRESULT CGameObject::Make_InstancePacket()
{
	INSTANCE_PACKET packet;
	packet.pModel = Get_Component<CInstanceModel>();
	packet.pMaterial = Get_Component<CMaterial>();
	packet.ObjID = m_ObjectID;

	if (!packet.pModel || !packet.pModel->Get_CompActive()) return E_FAIL;
	for (size_t i = 0; i < packet.pModel->Get_MeshCount(); i++)
	{
		if (!packet.pModel->isDrawable(i)) continue;
		packet.DrawIndex = i;
		packet.MaterialIndex = packet.pModel->Get_MaterialIndex(i);
		packet.pWorldMatrix = m_pTransform->Get_WorldMatrix_Ptr();
		CGameInstance::GetInstance()->Get_RenderSystem()->Submit_Instance(packet);
	}

	return S_OK;
}

HRESULT CGameObject::Make_ParticlePacket()
{
	CParticleSystem* pParticle = Get_Component<CParticleSystem>();

	PARTICLE_PACKET packet;
	if (pParticle->IsWorldSpace())
		packet.WorldMatrix = _smatrix::Identity;
	else
		packet.WorldMatrix = m_pTransform->Get_WorldMatrix();
	packet.ObjID = m_ObjectID;
	packet.pParticleSystem = Get_Component<CParticleSystem>();
	packet.pMaterial = Get_Component<CMaterial>();
	if (!packet.pParticleSystem || !packet.pMaterial) return E_FAIL;

	CGameInstance::GetInstance()->Get_RenderSystem()->Submit_Particle(packet);

	return S_OK;
}

HRESULT CGameObject::Make_EffectPacket(OPAQUE_PACKET packet)
{
	EFFECT_PACKET newPacket = {};
	newPacket.bSkinning = packet.bSkinning;
	newPacket.DrawIndex = packet.DrawIndex;
	newPacket.MaterialIndex = packet.MaterialIndex;
	newPacket.SkinningOffset = packet.SkinningOffset;
	newPacket.pModel = packet.pModel;
	newPacket.pMaterial = packet.pMaterial;
	newPacket.pPayLoad = packet.pPayLoad;
	newPacket.pWorldMatrix = packet.pWorldMatrix;
	//float3 toObj = objWorldPos - cameraPos;
	//float dist = dot(toObj, cameraForward);
	packet.ObjID = m_ObjectID;

	const _float4x4* viewInverseMat = CGameInstance::GetInstance()->Get_CameraMgr()->Get_InversedViewMatrix();
	_matrix viewInverse = XMLoadFloat4x4(viewInverseMat);

	_float4 camPos = CGameInstance::GetInstance()->Get_CameraMgr()->Get_CameraPos();

	_vector CamDist = m_pTransform->Get_Pos() - XMLoadFloat4(&camPos);
	_vector CamForward = XMVector4Normalize(viewInverse.r[2]);

	newPacket.DistanceToCamera = XMVectorGetX(XMVector4Dot(CamDist, CamForward));

	CGameInstance::GetInstance()->Get_RenderSystem()->Submit_Effect(newPacket);
	return S_OK;
}

HRESULT CGameObject::Make_3DUIPacket(OPAQUE_PACKET packet)
{
	CGameInstance::GetInstance()->Get_RenderSystem()->Submit_UI3D(packet);
	return S_OK;
}


void CGameObject::Free()
{
	__super::Free();

	for (auto& pair : m_Components) {

		//if (pair.first == type_index(typeid(CModel)))
		//	continue;
		//if (pair.first == type_index(typeid(CCollider)))
		//	continue;
		pair.second->Set_CompActive(false);

		pair.second->Releas_Component();
		Safe_Release(pair.second);
	}

	Safe_Release(m_pTransform);
}
