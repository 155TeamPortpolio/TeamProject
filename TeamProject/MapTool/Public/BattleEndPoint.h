#pragma once
#include "BattleObject.h"

NS_BEGIN(MapTool)
class CBattleEndPoint final : public CBattleObject
{
private:
    CBattleEndPoint();
    CBattleEndPoint(const CBattleEndPoint& rhs);
    virtual ~CBattleEndPoint() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Awake() override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;
    //virtual void Export_ObjectData(void* pDesc) override;

public:
    static CBattleEndPoint* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};
NS_END
