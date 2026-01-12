#pragma once
#include "MapObject.h"

NS_BEGIN(Client)
class CMapAnimObject :
    public CMapObject
{
private:
    CMapAnimObject();
    CMapAnimObject(const CMapAnimObject& rhs);
    virtual ~CMapAnimObject() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Awake() override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

public:
    void Render_GUI() override;

private:
    string  m_TagModelKey = {};
    string  m_TagMaterialKey = {};

public:
    static CMapAnimObject* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();
};
NS_END
