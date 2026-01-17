#pragma once
#include "MapObject.h"

NS_BEGIN(Client)
class CMapTriggerObject :
    public CMapObject 
{
private:
    enum class MapTriggerType { ENTER, STAY, EXIT, INTERECT, END };

private:
    CMapTriggerObject();
    CMapTriggerObject(const CMapTriggerObject& rhs);
    virtual ~CMapTriggerObject() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Awake() override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;
    virtual void OnTriggerEnter() override;

    virtual void OnTriggerExit() override;
    virtual void OnCollisionEnter() override;

    virtual void Export_ObjectData(void* pDesc) override;

public:
    void Render_GUI() override;

private:
    void Ready_PlaneUI(const MAPOBJ_DESC* pObjDesc);
    void Ready_MeshUI(const MAPOBJ_DESC* pObjDesc);
    void Ready_TriggerEvent(const MAPOBJ_DESC* pObjDesc);

private:
    MapTriggerType  m_eEventType{};
    string          m_EventTag{};

public:
    static CMapTriggerObject* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();
};
NS_END
