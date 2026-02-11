#pragma once
#include "Enemy.h"
NS_BEGIN(Client)
class CDefilerWall :
    public CEnemy
{
public:
    struct DefilerWallDesc : GAMEOBJECT_DESC{
        _vector3 vLook;
    };

private:
    CDefilerWall();
    CDefilerWall(const CDefilerWall& rhg);
    virtual ~CDefilerWall() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Awake() override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;
    void    Render_GUI()override;
public:
    virtual void    OnTriggerEnter(CGameObject* pOther) override;
    virtual void OnPooledAcquire(INIT_DESC* pArg = nullptr) override;		// 풀에서 꺼낼 때
    virtual void OnPooledRelease()override;

private:
    void DisAppear();

private:
    _float  m_ElapsedTime = 0.f;
    _float  m_fDissolveProgress = 0.f;

    _float2 m_vDissolveTiling = { 1.f, 1.f }; // 덩쿨 반복 크기
    _float  m_fDissolveScrollSpeed = 0.35f;          // 아래로 흐르는 속도
    _float  m_fDissolveNoiseStrength = 0.18f;          // 경계 흔들림
    _float  m_fDissolveEdgeWidth = 0.04f;          // 경계 부드러움
public:
    static CDefilerWall* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};
NS_END