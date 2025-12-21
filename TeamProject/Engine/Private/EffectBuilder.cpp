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
		m_CloneDesc = new CLONE_DESC(cloneDesc);
	}

	Safe_AddRef(m_pGameInstance);
	m_pObjDesc = new EFFECT_DESC;
}

CEffectBuilder::~CEffectBuilder()
{
	for (auto& pair : m_CompDesc)
		delete(pair.second);

	m_CompDesc.clear();
	Safe_Delete(m_CloneDesc);
	Safe_Delete(m_pObjDesc);

	Safe_Release(m_pGameInstance);
}

CEffectContainer* CEffectBuilder::Build(const string& instanceKey, _uint* id)
{
	if (m_CloneDesc->OriginLevel.empty())
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

	//프로토 매니저에서 가져오기
	CGameObject* object = m_pGameInstance->Get_PrototypeMgr()->Clone_Prototype(
		m_CloneDesc->OriginLevel, m_CloneDesc->protoTag,
		m_pObjDesc);
	CEffectContainer* instance = dynamic_cast<CEffectContainer*>(object);

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
