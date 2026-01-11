#pragma once
#include "EffectNode.h"
#include "TrailModel.h"

NS_BEGIN(Engine)
class ENGINE_DLL CTrailNode :
    public CEffectNode
{
protected:
    CTrailNode();
    CTrailNode(const CTrailNode& rhs);
    virtual ~CTrailNode() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Awake() override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

public:
    virtual void Play() override;
    virtual void Stop() override;

public:
    static CTrailNode* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

protected:
    CTrailModel::POINT_MODE m_eMode{};

};
NS_END
