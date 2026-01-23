#pragma once
#include "BattleObject.h"

NS_BEGIN(MapTool)
class CBattlePlayerPoint final : public CBattleObject
{
private:
    CBattlePlayerPoint();
    CBattlePlayerPoint(const CBattlePlayerPoint& rhs);
    virtual ~CBattlePlayerPoint() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Awake() override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;
    //virtual void Export_ObjectData(void* pDesc) override;

public:
    static CBattlePlayerPoint* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};
NS_END
