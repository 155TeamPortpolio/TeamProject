#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CUIDirector final : public CBase
{
	DECLARE_SINGLETON(CUIDirector);
private:
	CUIDirector() {}
	virtual ~CUIDirector() = default;

public:
	/* 화면 페이드인 실행 (검정 화면 -> 화면) */
	void FadeIn_Screen(_float fDuration = 0.5f);
	/* 화면 페이드아웃 실행 (화면 -> 검정 화면) */
	void FadeOut_Screen(_float fDuration = 0.5f);

	/* 전투 HUD를 화면에 표시 */
	void Show_BattleHUD(_bool isFade = true);
	/* 전투 HUD를 화면에서 숨김 */
	void Hide_BattleHUD();

	/* 씬 프레임을 화면에 표시 (화면 위, 아래에 프레임) */
	void Show_SceneFrame();
	/* 씬 프레임을 화면에서 숨김 (화면 위, 아래에 프레임) */
	void Hide_SceneFrame();

public:
	/* 모든 레벨에 필요한 공통 데이터 등록 */
	void Initialize();
	/* 레벨별로 필요한 프로토타입, 게임 오브젝트 등록 */
	void Load_LevelObjects(const string& levelKey);

private:
	/* json 파일에 저장된 레벨별 오브젝트 데이터를 읽고 저장 */
	void Load_UILevelData(const string& resourceKey);

private:
	string								m_levelKey;
	nlohmann::json						m_json = {};
	unordered_map<string, UI_HANDLE>	m_handles = {};

public:
	virtual void Free() override;
};

inline auto* UIDirector() { return CUIDirector::GetInstance(); }

NS_END