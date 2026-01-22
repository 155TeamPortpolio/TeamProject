#pragma once
#include "MapToolObject.h"

NS_BEGIN(MapTool)
class CBattleObject abstract : public CMapToolObject
{
public:
//    typedef struct tagBattleObjectCreateDesc : public CMapToolObject::MAPTOOL_OBJECT_DESC


protected:
    CBattleObject();
    CBattleObject(const CBattleObject& rhs);
    virtual ~CBattleObject() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Awake() override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;
    //virtual void Export_ObjectData(void* pDesc);

public:
    void Render_GUI() override;

public:
    virtual void Free() override;
};
NS_END
