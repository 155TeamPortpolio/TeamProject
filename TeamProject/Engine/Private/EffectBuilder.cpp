#include "Engine_Defines.h"
#include "EffectBuilder.h"
#include "GameInstance.h"
#include "EffectContainer.h"

CEffectBuilder::CEffectBuilder(const CLONE_DESC& cloneDesc)
	:m_pGameInstance(CGameInstance::GetInstance())
{
	if (!CGameInstance::GetInstance()->Get_LevelMgr()->Check_ValidateLevel(cloneDesc.OriginLevel))
	{
		MSG_BOX("Origin Level Tag is Invalidate : Builder");
	}
	else {
		m_CloneDesc = cloneDesc;
	}

	Safe_AddRef(m_pGameInstance);
	m_pObjDesc = new EFFECT_DESC;
}

CEffectBuilder::~CEffectBuilder()
{
	for (auto& pair : m_CompDesc)
		delete(pair.second);

	m_CompDesc.clear();
	Safe_Delete(m_pObjDesc);

	Safe_Release(m_pGameInstance);
}

CEffectContainer* CEffectBuilder::Build(const string& instanceKey, _uint* id)
{
	if (m_CloneDesc.OriginLevel.empty())
	{
		MSG_BOX("CLONE_DESC is missing : CEffectBuilder ");
		return nullptr;
	}

	//오브젝트 설명체 채우기
	if (!m_pObjDesc)
		m_pObjDesc = new EFFECT_DESC;

	m_pObjDesc->InstanceName = instanceKey;
	for (auto& pair : m_CompDesc)
		m_pObjDesc->CompDesc[pair.first] = pair.second;

	CGameObject* instance = nullptr;

	if (m_isFromPool)
	{
		instance = ObjectManager()->Acquire(m_CloneDesc, m_pObjDesc);
		if (!instance) return nullptr;
		instance->Set_FromPool(true);
		instance->OnPooledAcquire(m_pObjDesc);
	}
	else {
		instance = m_pGameInstance->Get_PrototypeMgr()->Clone_Prototype(
			m_CloneDesc.OriginLevel, m_CloneDesc.protoTag,
			m_pObjDesc);
		if (!instance) return nullptr;
		instance->Set_FromPool(false);
		instance->Awake();
	}

	CEffectContainer* container = dynamic_cast<CEffectContainer*>(instance);

	if (container && id) {
		*id = container->Get_ObjectID();
	}
	container->Awake();
	return container;
}
CEffectBuilder& CEffectBuilder::Asset(const string& assetKey)
{
	if (!m_pObjDesc)
		m_pObjDesc = new EFFECT_DESC;

	m_pObjDesc->EffectAssetKey = assetKey;

	return *this;	
}

CEffectBuilder& CEffectBuilder::Position(const _float3 position)
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

CEffectBuilder& CEffectBuilder::Scale(const _float3 scale)
{
	return *this;
}
