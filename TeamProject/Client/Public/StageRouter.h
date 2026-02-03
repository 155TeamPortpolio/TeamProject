#pragma once
#include "LevelObject.h"

NS_BEGIN(Client)
class CStageRouter :
    public CLevelObject
{
private:
	CStageRouter();
	~CStageRouter() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype()      override { __super::Initialize_Prototype(); return S_OK; };
    virtual HRESULT Initialize(INIT_DESC* pArg) override    { __super::Initialize(pArg); return S_OK;};
    virtual void    Awake()                     override    {}
    virtual void    Priority_Update(_float dt)  override    {};
    virtual void    Update(_float dt)           override    {};
    virtual void    Late_Update(_float dt)      override    {};

public:
    virtual void    Render_GUI() override;
public:
    void Ready_Stages();
    StageType Pop_StageType();

private:
    queue<StageType> m_StageQueue;

public:
	static CStageRouter* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg) override;
	virtual void Free();
};

NS_END;