#pragma once
#include "MapToolObject.h"

NS_BEGIN(MapTool)
class CEntityObject :
    public CMapToolObject
{
private:
    CEntityObject();
    CEntityObject(const CEntityObject& rhs);
    virtual ~CEntityObject() DEFAULT;

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

private:
    _int        m_iType = {};

public:
    static CEntityObject* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();
};
NS_END
