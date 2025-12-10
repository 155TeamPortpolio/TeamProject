#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class ENGINE_DLL CLevel abstract:
    public CBase
{
protected:
    CLevel(const string& LevelKey);
    virtual ~CLevel() DEFAULT;
     
public:
    virtual HRESULT Initialize() PURE;
    virtual HRESULT Awake(); /*레벨의 이니셜라이즈는 레벨이 확정 되기 전에 불러지니까, 레벨이 확정된 이후에 불러지는거 하나*/
    virtual void Update() PURE;
    virtual HRESULT Render(ID3D11DeviceContext* pContext);
    virtual HRESULT Render() PURE;
    const string& Get_Key() { return m_LevelKey; }

public:
    template<typename T, typename ...Args>
    T* Add_LevelObject(Args&& ...args);
    template<typename T>
    T* Get_LevelObject();
    template<typename T>
    HRESULT Remove_LevelObject();

protected:
    string m_LevelKey = {};
    unordered_map<type_index, class CLevelObject*> m_pLevelObjects;

public:
    virtual void Free() override;
};

NS_END

template<typename T, typename ...Args>
inline T* CLevel::Add_LevelObject(Args && ...args)
{
    T* Obj = Get_LevelObject<T>();
    if (Obj)
        return Obj;

    Obj = T::Create(forward<Args>(args)...);

    if (Obj) {
        m_pLevelObjects.insert({ type_index(typeid(T)), Obj });
        Obj->Set_Owner(this);
    }
    return Obj;
}

template<typename T>
inline T* CLevel::Get_LevelObject()
{
    auto iter = m_pLevelObjects.find(type_index(typeid(T)));

    if (iter != m_pLevelObjects.end())
        return static_cast<T*>(iter->second);

    return nullptr;
}

template<typename T>
inline HRESULT CLevel::Remove_LevelObject()
{
    auto iter = m_pLevelObjects.find(type_index(typeid(T)));

    if (iter != m_pLevelObjects.end()) {
        Safe_Release(iter->second);
        m_pLevelObjects.erase(iter);
        return S_OK;
    }
    else {
        return E_FAIL;
    }
}
