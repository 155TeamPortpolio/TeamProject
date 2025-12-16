#pragma once
#include "GameObject.h"

NS_BEGIN(MapTool)
class CMapToolObject abstract :
    public CGameObject
{
public:
    typedef struct tagStaticObjectCreateDesc : public Engine::GAMEOBJECT_DESC {
        string  TagModelKey = "";
        string  TagMaterialKey = "";
        _bool   isRayReceiver = {};
    }MAPTOOL_OBJECT_DESC;

protected:
    CMapToolObject();
    CMapToolObject(const CMapToolObject& rhs);
    virtual ~CMapToolObject() DEFAULT;

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
    void Delete_Object();

protected:
    string  m_TagModelKey = {};
    string  m_TagMaterialKey = {};

public:
    virtual void Free();
};
NS_END
