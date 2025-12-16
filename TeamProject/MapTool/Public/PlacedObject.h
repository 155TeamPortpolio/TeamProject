#pragma once
#include "GameObject.h"

NS_BEGIN(MapTool)
class CPlacedObject :
    public CGameObject
{
public:
    typedef struct tagStaticObjectCreateDesc : public Engine::GAMEOBJECT_DESC {
        string  TagModelKey = "";
        string  TagMaterialKey = "";
        _bool   isRayReceiver = {};
    }STATIC_OBJECT_DESC;
private:
    CPlacedObject();
    CPlacedObject(const CPlacedObject& rhs);
    virtual ~CPlacedObject() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Awake() override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

public:
    void Render_GUI() override;
    void Delete_Object();

private:
    string  m_TagModelKey = {};
    string  m_TagMaterialKey = {};

public:
    static CPlacedObject* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();
};
NS_END
