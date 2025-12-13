#pragma once
#include "SpriteNode.h"

NS_BEGIN(EffectTool)
class CSpriteNode_Edit :
    public CSpriteNode
{
private:
    CSpriteNode_Edit();
    CSpriteNode_Edit(const CSpriteNode_Edit& rhs);
    virtual ~CSpriteNode_Edit() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Awake() override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

public:
    void Render_GUI() override;

public:
    static CSpriteNode_Edit* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;

};
NS_END
