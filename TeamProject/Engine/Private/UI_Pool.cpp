#include "Engine_Defines.h"
#include "UI_Pool.h"

#include "GameInstance.h"
#include "UI_Object.h"

CUI_Pool::CUI_Pool()
{
}

CUI_Object* CUI_Pool::Acquire(const CLONE_DESC& cloneDesc, INIT_DESC* pArg, _bool& outFirst)
{
    auto& freeList = m_FreeByProto[cloneDesc.protoTag];

    if (!freeList.empty())
    {
        CUI_Object* objectPtr = freeList.back();
        freeList.pop_back();
        return objectPtr;
    }

    CGameObject* newObj =
        PrototypeManager()->Clone_Prototype(
            cloneDesc.OriginLevel, cloneDesc.protoTag, pArg);
    CUI_Object* UIObj = dynamic_cast<CUI_Object*>(newObj);
    outFirst = true;
    UIObj->Set_PoolKey(cloneDesc);

    return UIObj;
}

void CUI_Pool::Return(const CLONE_DESC& cloneDesc, CUI_Object* objectPtr)
{
    if (!objectPtr) return;
    m_FreeByProto[cloneDesc.protoTag].push_back(objectPtr);
}

void CUI_Pool::ClearAll()
{
    for (auto& pool : m_FreeByProto)
    {
        for (auto& obj : pool.second)
        {
            Safe_Release(obj);
        }
        pool.second.clear();
    }
    m_FreeByProto.clear();
}

void CUI_Pool::ClearByProto(const CLONE_DESC& cloneDesc)
{
    auto it = m_FreeByProto.find(cloneDesc.protoTag);
    if (it == m_FreeByProto.end()) return;

    for (CGameObject* objectPtr : it->second)
        Safe_Release(objectPtr);

    it->second.clear();
    m_FreeByProto.erase(it);
}

CUI_Pool* CUI_Pool::Create()
{
    return new CUI_Pool();
}

void CUI_Pool::Free()
{
    ClearAll();
}
