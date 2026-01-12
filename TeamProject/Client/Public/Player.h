//Player.h
#pragma once
#include "GameObject.h"

NS_BEGIN(Client)
class CPlayer : public CGameObject
{
private:
    struct KeyInput
    {
        _int x = 0;
        _int z = 0;

        void  Reset() { x = 0; z = 0; }
        _bool IsZero() const { return x == 0 && z == 0; }
        _bool operator!=(const KeyInput& other) const
        {
            return x != other.x || z != other.z;
        }
    };

    struct InputState
    {
        KeyInput current;
        KeyInput previous;
        KeyInput lastValid;
        KeyInput currentMove;
        KeyInput previousMove;
        _vector3 direction = {};
        _vector3 prevDirection = {};

        _float bufferTimer = 0.f;
        void ResetBuffer()
        {
            bufferTimer = 0.f;
            lastValid.Reset();
            previousMove.Reset();
            currentMove.Reset();
        }
        _bool IsMoving() const
        {
            return direction.x != 0.f || direction.z != 0.f;
        }
    };
private:
    CPlayer() {}
    CPlayer(const CPlayer& rhs);
    virtual ~CPlayer() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;
    virtual void    Render_GUI() override;

private:
    void Update_KeyInput();

private:
    //class CFieldPlayer*   m_pFieldPlayer;
    class CBattlePlayer*  m_pBattlePlayer = nullptr;

public:
    static CPlayer* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};
NS_END