#include "pch.h"
#include "UIDirector.h"

#include "GameInstance.h"
#include "UI_Object.h"
#include "UILoader.h"

IMPLEMENT_SINGLETON(CUIDirector);

void CUIDirector::Set_BulkStatus(const vector<UI_STATUS_BULK_DESC>& bulkStatus)
{
	auto pEventSystem = CGameInstance::GetInstance()->Get_EventSystem();

	for (const auto& status : bulkStatus)
	{
		UI_STATUS_DESC desc = {};
		desc.eOwner = status.eOwner;

		for (const auto& value : status.statusValues)
		{
			desc.eType = value.first;
			desc.fCurValue = value.second.fCurValue;
			desc.fMaxValue = value.second.fMaxValue;
			pEventSystem->Broadcast<UI_STATUS_DESC>({ desc });
		} 
	} 
}

void CUIDirector::Set_PlayerStatus(const vector<UI_PLAYER_STATUS_DESC>& playerStatus)
{
	auto pEventSystem = CGameInstance::GetInstance()->Get_EventSystem();

	for (const auto& status : playerStatus)
	{
		UI_STATUS_DESC desc = {};

		auto send = [&](UI_STATUS_TYPE type, const UI_STATUS_VALUE& value) 
			{
				desc.eType = type;
				desc.fCurValue = value.fCurValue;
				desc.fMaxValue = value.fMaxValue;
				pEventSystem->Broadcast<UI_STATUS_DESC>({ desc });
			};

		send(UI_STATUS_TYPE::HP, status.hp);
		send(UI_STATUS_TYPE::SPECIAL, status.special);
		send(UI_STATUS_TYPE::ULTIMATE, status.ultimate);
	}
}

void CUIDirector::Initialize()
{
	// ui 관련 이미지, 폰트, json 파일 리소스 매니저에 등록
	UILoader::Add_ResourcePath();

	// json 파일에 저장된 레벨별 오브젝트 데이터를 읽고 저장
	Load_UILevelData("levelData.json");
}

void CUIDirector::Load_LevelObjects(const string& levelKey)
{
	m_levelKey = levelKey;
	// 레벨에 프로토타입 등록
	UILoader::Add_Prototype(levelKey);

	// json 데이터에서 레벨에 있어야하는 객체 생성
	const json& levels = m_json["levels"];

	if (!levels.contains(levelKey))
		return;

	const json& objects = levels[levelKey]["objects"];

	for (const auto& obj : objects)
	{
		const string protoTag = obj["prototypeTag"];
		const string instName = obj["instanceName"];

		CUI_Object* uiObj = Builder::Create_UIObject({ levelKey, protoTag })
			.Build(instName);

		if (!uiObj)
		{
			MSG_BOX("Failed to Create UI Object : UI Director");
			continue;
		}

		if (FAILED(CGameInstance::GetInstance()->Get_UIMgr()->Add_UIObject(uiObj, levelKey)))
		{
			MSG_BOX("Failed to Add_UIObject : UI Director");
			continue;
		}

		UI_HANDLE handle = uiObj->Get_Handle();
		if (!handle.isValid())
		{
			MSG_BOX("Handle is not Vaild : UI Director");
			continue;
		}

		auto result = m_handles.emplace(instName, handle);
		if (!result.second)
			MSG_BOX("UI Object Already Exists : UI Director");
	}
}

void CUIDirector::Load_UILevelData(const string& resourceKey)
{
	string filePath = CGameInstance::GetInstance()->Get_ResourceMgr()->Get_ResourcePath(resourceKey);

	ifstream file(filePath);
	if (!file.is_open())
		return;

	file >> m_json;
	file.close();
}

void CUIDirector::Free()
{
	__super::Free();
}