#pragma once
#include "MapToolObject.h"
#include "MapTool_Struct.h"

NS_BEGIN(MapTool)
class CLightPoint :
    public CMapToolObject
{
public:
    typedef struct tagLightPointCreateDesc : public CMapToolObject::MAPTOOL_OBJECT_DESC
    {
        LIGHT_DESC_JSON DescJson = {};
    }LIGHT_INIT_DESC;

private:
    CLightPoint();
    CLightPoint(const CLightPoint& rhs);
    virtual ~CLightPoint() DEFAULT;

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
    LIGHT_DESC m_LightDesc;

public:
    static CLightPoint* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();
};
NS_END
