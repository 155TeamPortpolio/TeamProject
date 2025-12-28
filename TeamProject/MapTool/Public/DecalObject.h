#pragma once
#include "MapToolObject.h"

NS_BEGIN(MapTool)

class CDecalObject final : public CMapToolObject
{
private:
    CDecalObject();
    CDecalObject(const CDecalObject& rhs);
    virtual ~CDecalObject() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Awake() override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

    virtual void Export_ObjectData(void* pDesc) override;


public:
    void Render_GUI() override;

public:
    static CDecalObject* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();


};

NS_END