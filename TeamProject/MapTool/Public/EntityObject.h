#pragma once
#include "MapToolObject.h"

NS_BEGIN(MapTool)
class CEntityObject :
    public CMapToolObject
{
public:
    typedef struct tagEntityObjectCreateDesc : public CMapToolObject::MAPTOOL_OBJECT_DESC 
    {
        _int iType = {};
    }ENTITY_INIT_DESC;

    struct CurModel {
        _bool IsSkinned = { false };
        string ModelTag{};
        string ModelKey{};
        string MaterialKey{};
    };

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
    const CurModel* Get_CurrentModel() { return &m_tCurModel; };
    void Set_EntityModel(const string& ModelTag, const string& ModelKeyTag, const string& MaterialKeyTag);

private:
    string Get_TypeName();
    _vector4 Get_TypeColor();

public:
    void Render_GUI() override;

private:
    _int        m_iType = {};
    CurModel    m_tCurModel;

public:
    static CEntityObject* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};
NS_END
