#include "Engine_Defines.h"
#include "UIObjcetBuilder.h"
#include "GameInstance.h"

#include "IProtoService.h"
#include "ILevelService.h"
#include "IUI_Service.h"
#include "UI_Object.h"

CUIObjcetBuilder::CUIObjcetBuilder(const CLONE_DESC& cloneDesc)
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
	m_pObjDesc = new UI_DESC;
}

CUIObjcetBuilder::~CUIObjcetBuilder()
{
	for (auto& pair : m_CompDesc)
		delete(pair.second);

	m_CompDesc.clear();
	Safe_Delete(m_CloneDesc);
	Safe_Delete(m_pObjDesc);

	Safe_Release(m_pGameInstance);
}

CUI_Object* CUIObjcetBuilder::Build(const string& instanceKey, _uint* id)
{
	if (!m_CloneDesc)
	{
		MSG_BOX("CLONE_DESC is missing : CUIObjcetBuilder ");
		return nullptr;
	}

	if (m_LevelTag.empty())
	{
		MSG_BOX("LAYER_DESC is missing : CUIObjcetBuilder ");
		return nullptr;
	}

	//오브젝트 설명체 채우기
	m_pObjDesc->InstanceName = instanceKey;

	for (auto& pair : m_CompDesc)
		m_pObjDesc->CompDesc[pair.first] = pair.second;

	//프로토 매니저에서 가져오기
	CGameObject* Object = m_pGameInstance->Get_PrototypeMgr()->
		Clone_Prototype(m_CloneDesc->OriginLevel, m_CloneDesc->protoTag, m_pObjDesc);
	CUI_Object* instance = dynamic_cast<CUI_Object*>(Object);

	if (!instance) {
		return nullptr;
	}

	if(m_bPivoted)
		instance->Align_To(m_eAnchor, m_vPivot);

	if (instance && id) {
		*id = instance->Get_ObjectID();
	}

	instance->Awake();
	return instance;
}


CUIObjcetBuilder& CUIObjcetBuilder::Position(const _float2 position)
{
	m_pObjDesc->fX = position.x;
	m_pObjDesc->fY = position.y;
	return *this;
}

CUIObjcetBuilder& CUIObjcetBuilder::Rotate(const _float radian)
{
	m_pObjDesc->fRadian = radian;
	return *this;
}

CUIObjcetBuilder& CUIObjcetBuilder::Scale(const _float2 scale)
{
	m_pObjDesc->fSizeX = scale.x;
	m_pObjDesc->fSizeY = scale.y;
	return *this;
}
CUIObjcetBuilder& CUIObjcetBuilder::Set_Anchor(ANCHOR eAnchor, _float2 vPivot)
{
		m_eAnchor = eAnchor;
		m_vPivot = vPivot;
		m_bPivoted = true;
		return *this;
}