#pragma once
#include "Npc.h"

NS_BEGIN(Client)

class CDialogueNpc abstract :
    public CNpc
{
protected:
    CDialogueNpc();
    CDialogueNpc(const CDialogueNpc& rhs);
    virtual ~CDialogueNpc() DEFAULT;
public:
    void    Process_Event(const NPC_INTERACT_DESC& desc);

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;

protected:
    NpcIDDesc             m_DiagloueData;
    _uint                 m_iCurSequenceID = {};
    _uint                 m_iNextSequceID = {};

public:
    CGameObject* Clone(INIT_DESC* pArg) PURE;
    virtual void Free() override;
};

NS_END