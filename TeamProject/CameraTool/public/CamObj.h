#pragma once

NS_BEGIN(CameraTool)

class CamObj abstract : public CGameObject
{
protected:
    CamObj() : CGameObject(), transform(m_pTransform) {}
    CamObj(const CamObj& rhs) : CGameObject(rhs), transform(m_pTransform), game(rhs.game) {}
    virtual ~CamObj() = default;

public:
    HRESULT Initialize_Prototype()      override;
    HRESULT Initialize(INIT_DESC* pArg) override;

    void Priority_Update(_float dt) override PURE;
    void Update(_float dt)          override PURE;
    void Late_Update(_float dt)     override PURE;

protected:
    CGameInstance* game{};
    CCamera*       cam{};
    CamType        camType = CamType::Debug;
    CamRigType     rigType = CamRigType::Free;
    CTransform*&   transform;

public:
    CGameObject* Clone(INIT_DESC* pArg) override PURE;
    virtual void Free() override;
};

NS_END