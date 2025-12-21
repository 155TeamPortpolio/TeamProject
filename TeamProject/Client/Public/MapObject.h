#pragma once
#include "GameObject.h"
#include "MapData_Defines.h"

NS_BEGIN(Client)
class CMapObject abstract :
    public CGameObject
{
public:
    typedef struct tagStaticObjectCreateDesc : public Engine::GAMEOBJECT_DESC {
        string  TagLevel = "";
        string  TagModelKey = "";
        string  TagMaterialKey = "";
        _bool   bCooking = { false };
        unordered_map<string, vector<FIELD_DATA>>  SlotDataValues;
    }MAPOBJ_DESC;

protected:
    CMapObject();
    CMapObject(const CMapObject& rhs);
    virtual ~CMapObject() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Awake() override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

    virtual void Export_ObjectData(void* pDesc) PURE;

public:
    void Render_GUI() override;

protected:
    string  m_TagModelKey = {};
    string  m_TagMaterialKey = {};

public:
    virtual void Free();
};
NS_END
