#pragma once
#include "GameObject.h"
#include "Enemy_Struct.h"

NS_BEGIN(Engine)
class CAnimator3D;
NS_END

NS_BEGIN(Client)

class CEnemy abstract:
    public CGameObject
{
public:
    enum class BATTLE_COLTYPE{ ATTACK, TRIGGER };
protected:
    CEnemy();
    CEnemy(const CEnemy& rhg);
    virtual ~CEnemy() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Priority_Update(_float dt) override {};
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;

    virtual void OnPooledAcquire(INIT_DESC* pArg = nullptr) {}	// Ǯ���� ���� ��
    virtual void OnPooledRelease() {}							// Ǯ�� ���ư� ��


public: 
    /* Getter */
    // ���� �÷��� ���� ĳ������ ������ ��ȯ
    BATTLEOBJ_INFO*         GetCharacterOnField();
    // ���� �÷��� ���� ĳ���Ϳ��� �Ÿ������� ��ȯ
    TARGETING_INFO&         GetTargetingInfo() { return m_tTargetingInfo; }
    // ������ Status ����ü�� ��ȯ
    MONSTER_STATUS          GetStatus() { return m_tStatus; }
    // ������ Status ����ü �����͸� ��ȯ
    const MONSTER_STATUS*   GetStatusPtr() const { return &m_tStatus; }
    // Groggy ���� ��ȯ
    _bool                   IsGroggy() const { return m_isGroggy; }

    /* Setter*/
    // ���� ���� �� attack sign ����Ʈ Ȱ��ȭ �Լ�
    virtual void        Active_AttackSign(_bool parryEnable = true);
    // �÷��̾� ���⿡�� �������� ������ ���� �� ȣ�� �� �Լ�
    virtual void        TakeDamage(DAMAGE_TYPE eDamageType, _float fDamage) {};
    /* Ʈ���� �ݶ��̴��� �ٷ� Ű��, AttackOffsetTime �ڿ� Attack �ݶ��̴���
    AttackPlayTime��ŭ Ű�� Ʈ���ſ� Attack�ݶ��̴��� ������*/
    void                SetAutoPlayBattleCollider(const string& tagBattleCollider, _float fAttackOffsetTime, _float fAttackPlayTime, const HitDesc & hitDesc);

    void                Death();

protected:
    // Target(Player->Character)���� �Ÿ� ���� ���
    void                ComputeTargetingInfo();
    // Target(Player->Character)�� ���� ��, Target�� ������ Target���� ������ ���� GUI�� ����
    void                Render_GUI_ForTargetInfo();
    // Attack Sign ��ü �߰� �� ���� �� ����
    virtual void        Create_AttackSign(string boneTag);
    // BattleCollider ��ü �߰�
    HRESULT             AttachBattleColliderObject(BATTLE_COLLIDER_DESC* pDesc);
    // Groggy ��ġ ����
    void                ManageGroggy(const _float dt);

    void                Create_MeshPyramid();


    // Enemy Status ��ü �߰� �� ���� ���, �� ���� ��� �����ͷ� ����
    virtual void        Create_UIEnemyStatus(string boneTag);
    virtual void        Create_UIBossHUD();

#pragma region BattleCollider
protected:
    void                SetBattleColliderObject(const string& tagBattleColliderObject, BATTLE_COLTYPE eBattleColliderType, _bool is, const HitDesc & hitdesc);
    void                FinishBattleColliderObject(const string& tagBattleColliderObject);
    void                ShowBattleColliderForCheck(_bool is);
    void                CheckAutoBattlePlay(const _float dt);

    _bool               IsAliveBattleColliderObject(const string& tagBattleColliderObject, BATTLE_COLTYPE eBattleColliderType);

    unordered_map<string, _int> m_BattleColliderChildrenIndex;
    AUTO_BATTLECOL      m_tAutoBattleCol = {};

    // For.Debug
public:
    void                SetEnterAttackHit(_bool is) { m_isEnterAttackHit = is; }
    void                SetEnterTriggerHit(_bool is) { m_isEnterTriggerHit = is; }
protected:
    void                Render_GUI_ForShowBattleColliderHit();
protected:
    _bool               m_isEnterAttackHit = { false };
    _bool               m_isEnterTriggerHit = { false };
#pragma endregion

protected:
    UI_HANDLE               m_hUIEnemyStatus = {};

    // BattleSystem���� ���� ���� Character����
    vector<BATTLEOBJ_INFO>  m_PlayerCharacterInfos; 
    // Target(Player-Character)�� ���� ��, Target ������ ���� ����ü
    TARGETING_INFO          m_tTargetingInfo = {};
    // �÷��̾ �����ϴ� ��Ÿ� ����(���ݿ� ��Ÿ� Ȥ�� �߰ݿ����� ���)
    _float                  m_fDetectedRange = { 5.f };    
    // ���� �������ͽ�
    MONSTER_STATUS          m_tStatus = {};
    /* Groggy */
    _bool                   m_isGroggy = { false };
    _float                  m_fGroggyDecreaseTime = {};

 
protected: 
    virtual CGameObject* Clone(INIT_DESC* pArg) PURE;
    virtual void Free() override;
};
NS_END