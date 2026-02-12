#pragma once
#include "BasePanel.h"

NS_BEGIN(Client)
class CBattleSystem_Panel :
	public CBasePanel
{
private:
	CBattleSystem_Panel(GUI_CONTEXT* context);
	virtual ~CBattleSystem_Panel() DEFAULT;
    HRESULT Initialize() { return S_OK; };

public:
	virtual void Render_GUI() override;
	virtual void Update_Panel(_float dt) override;
private:
    // UI state
    int  m_selectedType = 0;         // BATTLE_OBJ_TYPE 선택
    int  m_selectedRow = -1;         // 목록 선택
    bool m_onlyOnField = true;
    bool m_onlyValid = true;
    bool m_autoRefresh = true;
    bool m_showIndexAudit = true;

    // spawn 테스트
    char  m_spawnProto[128] = "Proto_GameObject_MiasmaHeavy";
    float m_spawnPos[3] = { 0.f, 0.f, 0.f };
    float m_spawnRot[3] = { 0.f, 0.f, 0.f };

    // AoE 테스트
    float m_aoeCenter[3] = { 0.f, 0.f, 0.f };
    float m_aoeRadius = 4.f;
    float m_coneDir[3] = { 0.f, 0.f, 1.f };
    float m_coneAngle = 90.f;

    float m_boxCenter[3] = { 0.f, 0.f, 0.f };
    float m_boxHalf[3] = { 2.f, 2.f, 2.f };
    float m_boxRotEuler[3] = { 0.f, 0.f, 0.f };

    // hit 테스트
    int   m_damageType = 0;  // DAMAGE_TYPE
    float m_damage = 100.f;

    _bool m_bTabOpen = { false };
private:
    void DrawHeader();
    void DrawTab_Overview();
    void DrawTab_Objects();
    void DrawTab_IndexAudit();
    void DrawTab_Tools();

    const char* TypeName(BATTLE_OBJ_TYPE type) const;
public:
	static CBattleSystem_Panel* Create(GUI_CONTEXT* context);
	virtual void Free() override;
};
NS_END
