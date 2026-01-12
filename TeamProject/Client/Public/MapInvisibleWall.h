#pragma once
#include "MapObject.h"

NS_BEGIN(Client)
class CMapInvisibleWall :
    public CMapObject
{
private:
    CMapInvisibleWall();
    CMapInvisibleWall(const CMapInvisibleWall& rhs);
    virtual ~CMapInvisibleWall() DEFAULT;

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

public:
    static CMapInvisibleWall* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();
};
NS_END
