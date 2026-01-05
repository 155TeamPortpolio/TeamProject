#pragma once
#include "TrailNode.h"
#include "EffectContainer_Edit.h"

NS_BEGIN(EffectTool)
class CTrailNode_Edit :
    public CTrailNode
{
public:
    typedef struct tagTrailNodeEditDesc : public GAMEOBJECT_DESC
    {
        CEffectContainer_Edit::EFFECT_EDIT_CONTEXT* pContext = nullptr;
    }TRAIL_NODE_EDIT_DESC;
private:
    CTrailNode_Edit();
    CTrailNode_Edit(const CTrailNode_Edit& rhs);
    virtual ~CTrailNode_Edit() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Awake() override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

public:
    void Render_GUI()override;
    void Play()override;
    void Import(nlohmann::ordered_json& json);
    void Export(nlohmann::ordered_json& json);

public:
    static CTrailNode_Edit* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

private:
    CEffectContainer_Edit::EFFECT_EDIT_CONTEXT* m_pContext = nullptr;

};
NS_END
