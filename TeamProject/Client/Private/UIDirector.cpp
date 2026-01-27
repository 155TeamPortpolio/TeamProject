#include "pch.h"
#include "UIDirector.h"

#include "GameInstance.h"
#include "UI_Object.h"
#include "UILoader.h"
#include "UI_ScreenFade.h"
#include "UI_HUD.h"
#include "UI_DamageText.h"
#include "UI_ResultBanner.h"

IMPLEMENT_SINGLETON(CUIDirector);

void CUIDirector::FadeIn_Screen(_float fDuration)
{
	CUI_ScreenFade::FADE_DESC desc = {};
	desc.fDuration = fDuration;

	UI_Active("screen_fade", &desc);
}

void CUIDirector::FadeOut_Screen(_float fDuration)
{
	CUI_ScreenFade::FADE_DESC desc = {};
	desc.fDuration = fDuration;

	UI_DeActive("screen_fade", &desc);
}

void CUIDirector::Show_HUD(HUD hud, _bool isFade)
{
	Show_HUD(Get_HUDName(hud), isFade);
}

void CUIDirector::Hide_HUD(HUD hud)
{
	Hide_HUD(Get_HUDName(hud));
}

void CUIDirector::Show_SceneFrame()
{
	UI_Active("scene_frame");
}

void CUIDirector::Hide_SceneFrame()
{
	UI_DeActive("scene_frame");
}

void CUIDirector::Request_DamageText(void* pArg)
{
	if (!pArg) return;
	auto desc = static_cast<CUI_DamageText::DAMAGE_DESC*>(pArg);

	const string levelKey = LevelManager()->Get_NowLevelKey();

	auto dmgText = Builder::Create_UIObject({G_GlobalLevelKey, "Proto_GameObject_DamageText"}).FromPool().Build("DamageText");

	dmgText->UI_Active(desc);

	UIManager()->Add_UIObject(dmgText, levelKey);
}

void CUIDirector::Show_ResultBanner(const string& strTextureKey, const _wstring& wstrText1, const _wstring& wstrText2)
{
	CUI_ResultBanner::RESULT_DESC desc = {};
	desc.strTextureKey = strTextureKey;
	desc.wstrText1 = wstrText1;
	desc.wstrText2 = wstrText2;

	UI_Active("result_banner", &desc);
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
	m_handles.clear();

	m_levelKey = levelKey;
	// 레벨에 프로토타입 등록
	UILoader::Add_Prototype(levelKey);

	// json 데이터에서 레벨에 있어야하는 객체 생성
	const json& levels = m_json["levels"];

	if (!levels.contains(levelKey) && !levels.contains("Global_Level"))
		return;

	vector<string> targetKeys;

	if (levels.contains(levelKey))
		targetKeys.push_back(levelKey);

	if (levels.contains("Global_Level"))
		targetKeys.push_back("Global_Level");

	if (targetKeys.empty())
		return;

	for (const auto& key : targetKeys)
	{
		const json& objects = levels[key]["objects"];

		for (const auto& obj : objects)
		{
			const string protoTag = obj["prototypeTag"];
			const string instName = obj["instanceName"];
			const string prefabPath = obj["prefabPath"];

			auto builder = Builder::Create_UIObject({ key, protoTag });	// 나중에 아마도 대부분 글로벌, 그리고 몇몇개만 레벨별로?
			if (!prefabPath.empty())
				builder.Asset(prefabPath);

			CUI_Object* pObj = builder.Build(instName);

			if (!pObj)
			{
				MSG_BOX("Failed to Create UI Object : UI Director");
				continue;
			}

			if (FAILED(CGameInstance::GetInstance()->Get_UIMgr()->Add_UIObject(pObj, levelKey)))
			{
				MSG_BOX("Failed to Add_UIObject : UI Director");
				continue;
			}

			UI_HANDLE handle = pObj->Get_Handle();
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

void CUIDirector::Show_HUD(const string& strInstanceName, _bool isFade)
{
	auto it = m_handles.find(strInstanceName);
	if (it == m_handles.end() || !it->second.isValid())
		return;

	CUI_HUD::UI_TRANSITION_DESC desc = {};
	desc.isFade = isFade;
	it->second.Get()->UI_Active(&desc);
}

void CUIDirector::Hide_HUD(const string& strInstanceName)
{
	auto it = m_handles.find(strInstanceName);
	if (it == m_handles.end() || !it->second.isValid())
		return;

	it->second.Get()->UI_DeActive();
}

string CUIDirector::Get_HUDName(HUD hud)
{
	switch (hud)
	{
	case HUD::FIELD:		return "hud_field";
	case HUD::BATTLE:		return "hud_battle";
	}
	return "";
}

void CUIDirector::UI_Active(const string& strInstanceName, void* pArg)
{
	auto it = m_handles.find(strInstanceName);
	if (it == m_handles.end() || !it->second.isValid())
		return;

	it->second.Get()->UI_Active(pArg);
}

void CUIDirector::UI_DeActive(const string& strInstanceName, void* pArg)
{
	auto it = m_handles.find(strInstanceName);
	if (it == m_handles.end() || !it->second.isValid())
		return;

	it->second.Get()->UI_DeActive(pArg);
}

void CUIDirector::Free()
{
	__super::Free();
}