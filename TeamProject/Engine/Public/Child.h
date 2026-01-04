#include"Component.h"

NS_BEGIN(Engine)
class ENGINE_DLL CChild :
    public CComponent {
    friend class CObjectContainer;
private:
    CChild(class CGameObject* pParent);
    CChild(const CChild& rhs);
    virtual ~CChild() override DEFAULT;
public:
    HRESULT Initialize_Prototype();
    HRESULT Initialize(COMPONENT_DESC* pArg) override;
    void Sync_To_Parent(_bool SyncTransform);
    class CGameObject* Get_Parent() { return m_pParent; };

public:
    void ReorderChildren(class CGameObject* pObject, _uint Index);
    void Upper_Order(class CGameObject* pObject);
    void Lower_Order(class CGameObject* pObject);
    void Set_Order_First(class CGameObject* pObject);
    void Set_Order_Last(class CGameObject* pObject);

private:
    void Set_Parent(class CGameObject* pParent);
    void Dettach_Parent();

public:
    static CChild* Create(class CGameObject* pParent);
    virtual void Free() override;
private:
    class CGameObject* m_pParent = { nullptr };
private:
    virtual CComponent* Clone();
};

NS_END