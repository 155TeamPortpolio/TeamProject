#pragma once
#include "Engine_Defines.h"
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

protected:
	unordered_set<ICollidable*> m_CurrentCollisions;
	unordered_set<ICollidable*> m_PreviousCollisions;
	_int  m_iSlotIndex = { -1 };
	_uint m_iSlotGeneration = { 0 };
	COLLISION_GROUP m_eGroup = { COLLISION_GROUP::COMMON };
public:
	virtual void Free() override { __super::Free(); }
};

NS_END


namespace CollisionHelper
{
    // 충돌 그룹 이름 배열
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
        default:                             return "UNKNOWN";
        }
    }

    // 충돌 마스크 GUI 렌더링
    inline _bool RenderCollisionMaskEditor(const char* label, _uint& iMask)
    {
        _bool bChanged = false;

        if (ImGui::TreeNode(label))
        {
            const COLLISION_GROUP groups[] = {
                COLLISION_GROUP::COMMON,
                COLLISION_GROUP::PLAYER,
                COLLISION_GROUP::MONSTER,
                COLLISION_GROUP::PLAYER_ATTACK,
                COLLISION_GROUP::MONSTER_ATTACK,
                COLLISION_GROUP::MONSTER_PARRY,
                COLLISION_GROUP::CAMERA
            };

            for (auto eGroup : groups)
            {
                _uint bit = ENUM(eGroup);
                _bool bEnabled = (iMask & bit) != 0;

                if (ImGui::Checkbox(GetCollisionGroupName(eGroup), &bEnabled))
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

    // 충돌 마스크 읽기 전용 표시
    inline void RenderCollisionMaskReadOnly(const char* label, _uint iMask)
    {
        if (ImGui::TreeNode(label))
        {
            const COLLISION_GROUP groups[] = {
                COLLISION_GROUP::COMMON,
                COLLISION_GROUP::PLAYER,
                COLLISION_GROUP::MONSTER,
                COLLISION_GROUP::PLAYER_ATTACK,
                COLLISION_GROUP::MONSTER_ATTACK,
                COLLISION_GROUP::MONSTER_PARRY,
                COLLISION_GROUP::CAMERA
            };

            for (auto eGroup : groups)
            {
                _uint bit = ENUM(eGroup);
                _bool bEnabled = (iMask & bit) != 0;

                if (bEnabled)
                    ImGui::TextColored(ImVec4(0.2f, 1.f, 0.2f, 1.f), "[O] %s", GetCollisionGroupName(eGroup));
                else
                    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.f), "[X] %s", GetCollisionGroupName(eGroup));
            }

            ImGui::TreePop();
        }
    }
}