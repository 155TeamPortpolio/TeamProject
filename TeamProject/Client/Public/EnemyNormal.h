#pragma once
#include "Enemy.h"

NS_BEGIN(Client)

typedef struct tagRotateToPlayerDesc {
    _bool   isLookPlayer = { false };
    _float3 vDirToLookCapture = {};
    _float  fRotateSpeed = { 10.f };
}ROT_DIR;

class CEnemyNormal abstract : public CEnemy
{
protected:
    CEnemyNormal();
    CEnemyNormal(const CEnemyNormal& rhg);
    virtual ~CEnemyNormal() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    virtual void    Priority_Update(_float dt) override {};
    virtual void    Update(_float dt) override;
    virtual void    Late_Update(_float dt) override;

public:
    void            CaptureRotateToDir(_float3 vTargetDir, _float fSpeed = 0.5f);

protected:
    void            RotateToDir(_float dt);

protected:
    ROT_DIR         m_tRotDir = {};

protected:
    virtual void Free() override;
};

NS_END