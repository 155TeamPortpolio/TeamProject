#pragma once
#include "Base.h"

NS_BEGIN(Client)

class CFieldPlayer final:
    public CBase
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
        _float   bufferTimer = 0.f;

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
    CFieldPlayer();
    virtual ~CFieldPlayer() DEFAULT;

public:
    OBJECT_HANDLE Get_CurCharacterHandle();

public:
    HRESULT Initialize();
    void Priority_Update(_float dt);
    void Update(_float dt);
    void Late_Update(_float dt);

private:
    HRESULT                     Initialize_CharacterPrototype();
    class CFieldCharacter*      Create_Character();

    void    Update_Input(_float dt);
    void    Process_Movement(_float dt);

private:
    class CFieldCharacter*  m_pCurrentCharacter = nullptr;

private:
    InputState   m_input;
    static constexpr _float KEY_BUFFER_TIME = 0.1f;

public:
    static CFieldPlayer* Create();
    virtual void Free() override;
};

NS_END