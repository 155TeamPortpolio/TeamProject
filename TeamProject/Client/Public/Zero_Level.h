#pragma once
#include "Level.h"

NS_BEGIN(Engine)
class CGameInstance; 
class CAudioSource;
NS_END

NS_BEGIN(Client)
class CCamDirector;

struct StageNode {
	StageType		MyType;
	_int			ParentIndex;
	vector<_int>	ChildrenIndex; 
	_int depth = 0;
	_bool cleared = { false };
	_bool opened = { false };
};

struct MapCycle
{
	vector<string> maps;
	_int cursor = 0; /*ÇöÀç*/

	string Next()
	{
		if (maps.empty()) return {};
		if (cursor < 0) cursor = 0;
		string key = maps[cursor % (int)maps.size()];
		cursor = (cursor + 1) % (int)maps.size();
		return key;
	}
};

typedef struct tagStageContext {
	_bool isFirstIn = false;
	OBJECT_HANDLE hPlayer;
	CStage* pNowStage = { nullptr };
	StageType nextType = StageType::Normal;
	string mapKey;
}StageContext;

class CZero_Level : public CLevel
{
public:
	enum class ZeroUpdate { Nope, Target, RollBack };

	struct Zero_Fog {
		ZeroUpdate eUpdate = { ZeroUpdate::Nope };
		_bool  isDirty = false;
		_float fElapsed = 0.f;
		_float fDuration = 0.f;
		EaseType eEase = EaseType::None;
		FOG_DESC tBaseFog{}, tCurFog{}, tTargetFog{};

		void Update_Fog(_float dt);
		void Set_BaseFog(FOG_DESC FogDesc);
		void Change_FogState(FOG_DESC FogDesc, _float fTime, EaseType eEaseType);
		void RollBack_Fog(_float fTime, EaseType eEaseType);
		void Use_Fog(_bool b);
	};

	struct Zero_Cloud {
		ZeroUpdate eUpdate = { ZeroUpdate::Nope };
		_bool  isDirty = false;
		_float fElapsed = 0.f;
		_float fDuration = 0.0f;
		EaseType eEase = EaseType::None;
		CLOUD_DESC tBaseCloud{}, tCurCloud{}, tTargetCloud{};

		void Update_Cloud(_float dt);
		void Set_BaseCloud(CLOUD_DESC CloudDesc);
		void Change_CloudState(CLOUD_DESC CloudDesc, _float fTime, EaseType eEaseType);
		void RollBack_Cloud(_float fTime, EaseType eEaseType);
		void Set_Moon(_bool b);
		void Use_Cloud(_bool b);
	};

	struct Zero_Shadow {
		CGameObject* pShadowCam = { nullptr };

		void Set_ShadowPos(_vector3 vPosition);
		void Set_Light(LIGHT_DESC LightDesc);
	};

private:
	CZero_Level(const string& LevelKey);
	virtual ~CZero_Level() DEFAULT;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Awake()      override;
	virtual void    Update()     override;
	virtual HRESULT Render()     override;

public:
	StageContext& Get_StageContext() { return m_Context; };
	HRESULT ChangeStage(StageType type = StageType::Normal);
	string PopMapKey(StageType type);

	class CStageRouter* Get_Router() { return m_pRouter; }

private:
	void Ready_Prototype();
	void Ready_Stage();
	void Shuffle_MapCycle(vector<string>& Map);

private:
	StageContext m_Context = {};
	class CStageRouter* m_pRouter = { nullptr };
	unordered_map<StageType, class CStage*> m_StageContainer;
	unordered_map<StageType, MapCycle> m_mapCycle;

public:
	class CAudioSource* Get_ZeroBGM() { return m_pBGM; };
	Zero_Fog* Get_ZeroFog() { return &m_tZeroFog; };
	Zero_Cloud* Get_ZeroCloud() { return &m_tZeroCloud; };
	Zero_Shadow* Get_ZeroShadow() { return &m_tZeroShadow; };

private:
	class CAudioSource* m_pBGM = nullptr;
	Zero_Fog	m_tZeroFog;
	Zero_Cloud	m_tZeroCloud;
	Zero_Shadow m_tZeroShadow;

public:
	static CZero_Level* Create(const string& LevelKey);
	virtual void Free() override;
};

NS_END