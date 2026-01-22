#pragma once
#include "BattleObject.h"

NS_BEGIN(MapTool)
class CBattleSpawnerPoint final : public CBattleObject
{
private:
    CBattleSpawnerPoint();
    CBattleSpawnerPoint(const CBattleSpawnerPoint& rhs);
    virtual ~CBattleSpawnerPoint() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Awake() override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;
    void Render_GUI() override;
    virtual void Export_ObjectData(void* pDesc) override;

public:
    static CBattleSpawnerPoint* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};
NS_END
