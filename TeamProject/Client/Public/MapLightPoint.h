#pragma once
#include "MapObject.h"

NS_BEGIN(Client)
class CMapLightPoint :
    public CMapObject
{
public:
    typedef struct tagMapLightPoint : public CMapObject::MAPOBJ_DESC
    {
        LIGHT_DESC_JSON DescJson = {};
    }MAP_LIGHTPOINT_DESC;

private:
    CMapLightPoint();
    CMapLightPoint(const CMapLightPoint& rhs);
    virtual ~CMapLightPoint() DEFAULT;

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
    string  m_TagModelKey = {};
    string  m_TagMaterialKey = {};

public:
    static CMapLightPoint* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();
};
NS_END
