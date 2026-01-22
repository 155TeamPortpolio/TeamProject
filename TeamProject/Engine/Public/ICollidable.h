#pragma once
#include "Engine_Defines.h"
#include "Helper_Func.h"
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL ICollidable abstract : public CComponent
{
protected:
    ICollidable() = default;
	ICollidable(const ICollidable& rhs) : CComponent(rhs) {};
    virtual ~ICollidable() = default;

public:
    virtual void OnCollisionEnter(ICollidable* pOther) PURE;
    virtual void OnCollisionStay(ICollidable* pOther) PURE;
    virtual void OnCollisionExit(ICollidable* pOther) PURE;
	virtual void OnTriggerEnter(ICollidable* pOther) PURE;
	virtual void OnTriggerStay(ICollidable* pOther) PURE;
	virtual void OnTriggerExit(ICollidable* pOther) PURE;

	virtual void Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor) PURE;
    _vector4 Get_ColliderColor() { return m_vColor; }
    void    Set_ColliderColor(_vector4 vColor) { m_vColor = vColor; }

	_bool IsColliding() const { return !m_CurrentCollisions.empty(); }
	_bool IsCollidingWith(ICollidable* pOther) const
	{
		return m_CurrentCollisions.find(pOther) != m_CurrentCollisions.end();
	}
	unordered_set<ICollidable*>& Get_CurrentCollisions() { return m_CurrentCollisions; }
	unordered_set<ICollidable*>& Get_PreviousCollisions() { return m_PreviousCollisions; }
	virtual void Update_Collisions() { m_PreviousCollisions = m_CurrentCollisions;}
	
	void Set_SlotInfo(_int iIndex, _uint iGeneration)
	{
		m_iSlotIndex = iIndex;
		m_iSlotGeneration = iGeneration;
	}
	_int  Get_SlotIndex() const { return m_iSlotIndex; }
	_uint Get_SlotGeneration() const { return m_iSlotGeneration; }

    COLLISION_GROUP Get_Group() const { return m_eGroup; }
    _uint Get_CollisionMask() const { return m_iCollisionMask; }
    virtual void Set_CollisionGroup(COLLISION_GROUP eGroup) PURE;
    virtual void Set_CollisionMask(_uint iMask) PURE;

protected:
	unordered_set<ICollidable*> m_CurrentCollisions;
	unordered_set<ICollidable*> m_PreviousCollisions;

	_int  m_iSlotIndex = { -1 };
	_uint m_iSlotGeneration = { 0 };
    _uint m_iCollisionMask = { 0xFFFFFFFF };
    COLLISION_GROUP m_eGroup = { COLLISION_GROUP::COMMON };

    _vector4 m_vColor = { 0.f, 0.5019608f, 0.f, 1.f };

public:
	virtual void Free() override { __super::Free(); }
};

NS_END


namespace CollisionHelper
{
    // 유효한 충돌 그룹 목록 (END 제외)
    inline constexpr COLLISION_GROUP VALID_GROUPS[] = {
        COLLISION_GROUP::COMMON,
        COLLISION_GROUP::PLAYER,
        COLLISION_GROUP::MONSTER,
        COLLISION_GROUP::PLAYER_ATTACK,
        COLLISION_GROUP::MONSTER_ATTACK,
        COLLISION_GROUP::MONSTER_PARRY,
        COLLISION_GROUP::CAMERA,
        COLLISION_GROUP::INTERACTABLE
    };
    inline constexpr _int VALID_GROUP_COUNT = IM_ARRAYSIZE(VALID_GROUPS);

    inline const char* GetCollisionGroupName(COLLISION_GROUP eGroup)
    {
        switch (eGroup)
        {
        case COLLISION_GROUP::COMMON:        return "COMMON";
        case COLLISION_GROUP::PLAYER:        return "PLAYER";
        case COLLISION_GROUP::MONSTER:       return "MONSTER";
        case COLLISION_GROUP::PLAYER_ATTACK: return "PLAYER_ATTACK";
        case COLLISION_GROUP::MONSTER_ATTACK:return "MONSTER_ATTACK";
        case COLLISION_GROUP::MONSTER_PARRY: return "MONSTER_PARRY";
        case COLLISION_GROUP::CAMERA:        return "CAMERA";
        case COLLISION_GROUP::INTERACTABLE:   return "INTERACABLE";
        default:                             return "UNKNOWN";
        }
    }

    // Combo용 문자열 배열 자동 생성
    inline const char** GetGroupNames()
    {
        static const char* names[VALID_GROUP_COUNT] = {};
        static bool bInit = false;
        if (!bInit)
        {
            for (_int i = 0; i < VALID_GROUP_COUNT; ++i)
                names[i] = GetCollisionGroupName(VALID_GROUPS[i]);
            bInit = true;
        }
        return names;
    }

    // 현재 그룹의 인덱스 반환
    inline _int GetGroupIndex(COLLISION_GROUP eGroup)
    {
        for (_int i = 0; i < VALID_GROUP_COUNT; ++i)
        {
            if (VALID_GROUPS[i] == eGroup)
                return i;
        }
        return 0;
    }

    // Collision Group Combo 렌더링
    inline _bool RenderCollisionGroupCombo(const char* label, COLLISION_GROUP& eGroup)
    {
        _int iCurrentIndex = GetGroupIndex(eGroup);
        if (ImGui::Combo(label, &iCurrentIndex, GetGroupNames(), VALID_GROUP_COUNT))
        {
            eGroup = VALID_GROUPS[iCurrentIndex];
            return true;
        }
        return false;
    }

    // 충돌 마스크 GUI (기존 코드 수정)
    inline _bool RenderCollisionMaskEditor(const char* label, _uint& iMask)
    {
        _bool bChanged = false;
        if (ImGui::TreeNode(label))
        {
            for (_int i = 0; i < VALID_GROUP_COUNT; ++i)
            {
                _uint bit = ENUM(VALID_GROUPS[i]);
                _bool bEnabled = (iMask & bit) != 0;
                if (ImGui::Checkbox(GetCollisionGroupName(VALID_GROUPS[i]), &bEnabled))
                {
                    if (bEnabled)
                        iMask |= bit;
                    else
                        iMask &= ~bit;
                    bChanged = true;
                }
            }
            ImGui::TreePop();
        }
        return bChanged;
    }

    inline void RenderCollisionMaskReadOnly(const char* label, _uint iMask)
    {
        if (ImGui::TreeNode(label))
        {
            for (_int i = 0; i < VALID_GROUP_COUNT; ++i)
            {
                _uint bit = ENUM(VALID_GROUPS[i]);
                _bool bEnabled = (iMask & bit) != 0;
                if (bEnabled)
                    ImGui::TextColored(ImVec4(0.2f, 1.f, 0.2f, 1.f), "[O] %s", GetCollisionGroupName(VALID_GROUPS[i]));
                else
                    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.f), "[X] %s", GetCollisionGroupName(VALID_GROUPS[i]));
            }
            ImGui::TreePop();
        }
    }
}