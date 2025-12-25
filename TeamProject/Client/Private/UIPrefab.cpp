#include "pch.h"
#include "UIPrefab.h"

#include "GameInstance.h"

CUIPrefab::CUIPrefab()
{
}

CUIPrefab::CUIPrefab(const CUIPrefab& rhs)
	: CUI_Object(rhs)
{
}

HRESULT CUIPrefab::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CUIPrefab::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);

	UI_ELEMENT_DATA data = Helper::LoadJson<UI_ELEMENT_DATA>(
		CGameInstance::GetInstance()->Get_ResourceMgr()->Get_ResourcePath(pDesc->UIAssetKey));

	const string& strCurrentLevelKey = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();
	const string& strTypeTag = data.strTypeTag;
	CUI_Object* pObj = Builder::Create_UIObject({ strCurrentLevelKey , "Proto_GameObject_" + strTypeTag })
		.Build(strTypeTag);

	if (pObj)
		pObj->ReadElementData(data);

	CGameInstance::GetInstance()->Get_UIMgr()->Add_UIObject(pObj, strCurrentLevelKey);

	return S_OK;
}

void CUIPrefab::Priority_Update(_float dt)
{
}

void CUIPrefab::Update(_float dt)
{
}

void CUIPrefab::Late_Update(_float dt)
{
}

void CUIPrefab::LoadUIPrefab(const string& strResourceKey)
{
	UI_ELEMENT_DATA data = Helper::LoadJson<UI_ELEMENT_DATA>(CGameInstance::GetInstance()->Get_ResourceMgr()->Get_ResourcePath(strResourceKey));

	const string& strCurrentLevelKey = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();
	const string& strTypeTag = data.strTypeTag;
	CUI_Object* pObj = Builder::Create_UIObject({ strCurrentLevelKey , "Proto_GameObject_" + strTypeTag })
		.Build(strTypeTag);

	if (pObj)
		pObj->ReadElementData(data);

	CGameInstance::GetInstance()->Get_UIMgr()->Add_UIObject(pObj, strCurrentLevelKey);
}

void CUIPrefab::Free()
{
	__super::Free();
}