#pragma once
#include "MapObject.h"

NS_BEGIN(Client)
class CMapPlacedObject :
    public CMapObject
{
private:
    CMapPlacedObject();
    CMapPlacedObject(const CMapPlacedObject& rhs);
    virtual ~CMapPlacedObject() DEFAULT;

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
    void Delete_Object();

private:
    string  m_TagModelKey = {};
    string  m_TagMaterialKey = {};

public:
    static CMapPlacedObject* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();
};
NS_END
