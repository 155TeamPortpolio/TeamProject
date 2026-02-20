#pragma once
#include "Npc.h"
#include "NPC_Preset.h"

NS_BEGIN(Client)
class CCrowdNpc :
    public CNpc
{
public:
    struct CrowdNpcDesc : GAMEOBJECT_DESC {
        wstring Text;
    };

protected:
    CCrowdNpc();
    CCrowdNpc(const CCrowdNpc& rhs);
    virtual ~CCrowdNpc() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;
    virtual void    Render_GUI() override;

private:
    NpcColorPreset colorPreset;
    NpcModelPreset modelPreset;

public:
    static CCrowdNpc* Create();
    CGameObject* Clone(INIT_DESC* pArg);
    virtual void Free() override;
};

NS_END