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

private:
    void ColliderGroup_SlotData(MAPOBJ_DESC* pObjDesc);
    void Effect_SlotData(MAPOBJ_DESC* pObjDesc);
    void Rotate_SlotData(MAPOBJ_DESC* pObjDesc);
    void Animation_SlotData(MAPOBJ_DESC* pObjDesc);

private:
    void RotatePerSec(_float dt);
    void Wave(_float dt);
    void LookSway(_float dt);

public:
    void Render_GUI() override;

private:
    string  m_TagModelKey = {};
    string  m_TagMaterialKey = {};

private:
    //Rotate
    _vector3 m_vDegreePerSec = {};

    //Wave
    _vector2 m_vWave = {};
    _float   m_fBaseY{};
    _float   m_fWaveTime = -1.f;

    //LookSway
    _vector2 m_vLookSway = {};
    _vector4 m_BaseRotation;
    _float   m_fLookSwayTime = -1.f;

public:
    static CMapPlacedObject* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();
};
NS_END
