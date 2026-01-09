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

	if (!m_pObjDesc)
		m_pObjDesc = new UI_DESC;

	m_pObjDesc->InstanceName = instanceKey;

	for (auto& pair : m_CompDesc)
		m_pObjDesc->CompDesc[pair.first] = pair.second;

	CGameObject* Object = m_pGameInstance->Get_PrototypeMgr()->
		Clone_Prototype(m_CloneDesc->OriginLevel, m_CloneDesc->protoTag, m_pObjDesc);
	CUI_Object* instance = dynamic_cast<CUI_Object*>(Object);

	if (!instance) {
		return nullptr;
	}

	if (instance && id) {
		*id = instance->Get_ObjectID();
	}

	instance->Awake();
	return instance;
}


CUIObjcetBuilder& CUIObjcetBuilder::Offset(const _float2 Offset)
{
	m_pObjDesc->AnchorOffset = Offset;
	return *this;
}

CUIObjcetBuilder& CUIObjcetBuilder::Rotate(const _float radian)
{
	m_pObjDesc->fRadian = radian;
	return *this;
}

CUIObjcetBuilder& CUIObjcetBuilder::Scale(const _float2 scale)
{
	m_pObjDesc->Scale = scale;
	return *this;
}
CUIObjcetBuilder& CUIObjcetBuilder::Size(const _float2 size)
{
	m_pObjDesc->Size = size;
	return *this;
}

CUIObjcetBuilder& CUIObjcetBuilder::Anchor(ANCHOR eAnchor)
{
	m_pObjDesc->eAnchor = eAnchor;
	return *this;
}

CUIObjcetBuilder& CUIObjcetBuilder::Asset(const string& assetKey)
{
	m_pObjDesc->UIAssetKey = assetKey;
	return *this;
}

CUIObjcetBuilder& CUIObjcetBuilder::Add_UIDesc(UI_DESC* pArg)
{
	if (pArg == nullptr) return *this;
	m_pObjDesc = pArg;
	return *this;
}