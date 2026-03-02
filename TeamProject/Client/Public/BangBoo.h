#pragma once
#include "Npc.h"

NS_BEGIN(Client)

class CBangBoo :
    public CNpc
{
private:
    CBangBoo();
    CBangBoo(const CBangBoo& rhs);
    virtual ~CBangBoo() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Awake() override;
    virtual void    Priority_Update(_float dt) override;
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;
    
public:
    void Set_BangBoo_Model(const string& BangBooTag);
    void Set_BangBoo_Animation(const string& AnimationTag);
    void Set_BangBoo_Name(const wstring& NameTag);
    void Set_BangBoo_Speech(const wstring& SpeechString);

public:
    static CBangBoo* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END