#pragma once
#include "Interactable.h"
#include "IInteract.h"

NS_BEGIN(Client)

class CZeroPortal final :
    public CInteractable, public IInteract
{
public:
    typedef struct tagZeroPortalDesc : public Engine::GAMEOBJECT_DESC {
        string NextMapTag{};
    }ZEROPORTAL_DESC;

private:
    CZeroPortal();
    CZeroPortal(const CZeroPortal& rhs);
    virtual ~CZeroPortal() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype()      override;
    virtual HRESULT Initialize(INIT_DESC* pArg) override;
    void            Awake() override;
    virtual void    Priority_Update(_float dt)  override;
    virtual void    Update(_float dt)           override;
    virtual void    Late_Update(_float dt)      override;

    virtual void Render_GUI()override;

    virtual void    OnTriggerEnter(CGameObject* pOther) override;
    virtual void    OnTriggerStay(CGameObject* pOher)   override;
    virtual void    OnTriggerExit(CGameObject* pOther)  override;

    virtual void                Interact(CGameObject* pObject = nullptr) override;
    virtual OBJECT_HANDLE       Get_InteractHandle() override;

public:
    void SetChoiceIndex(class CStage* pOwener, int idx);
    int  GetChoiceIndex() const { return m_choiceIndex; }

private:
    void Extend(_float dt);
    void Contract(_float dt);

private:
    _bool  m_bVisible = { false };
    string m_NextMapTag{};
    _float m_Time = {};

private:
    _int m_choiceIndex = -1;
    class CStage* m_pOwnerStage = { nullptr };

private:
    _bool m_OnExtend = false;
    _bool m_OnContract = false;

    _float m_fDuration{};
    _float m_fElapsedTime{};

    _float3 m_vContractScale{ 0.3f,0.3f,0.3f };
    _float3 m_vExtendScale{ 1.3f,1.3f,1.3f };
public:
    static CZeroPortal* Create();
    virtual CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END