#include "Engine_Defines.h"
#include "ObjectPool.h"
#include "GameInstance.h"

CObjectPool::CObjectPool()
{
}

CGameObject* CObjectPool::Acquire(const CLONE_DESC& cloneDesc)
{
    auto& freeList = m_FreeByProto[cloneDesc.protoTag];

    if (!freeList.empty())
    {
        CGameObject* objectPtr = freeList.back();
        freeList.pop_back();
        return objectPtr;
    }

    GAMEOBJECT_DESC emptyDesc;
    emptyDesc.InstanceName = "Pooled";

    CGameObject* newObj =
        CGameInstance::GetInstance()->Get_PrototypeMgr()->Clone_Prototype(
            cloneDesc.OriginLevel, cloneDesc.protoTag, &emptyDesc);

    newObj->Set_PoolKey(cloneDesc);

    return newObj;
}

void CObjectPool::Return(const CLONE_DESC& cloneDesc, CGameObject* objectPtr)
{
    if (!objectPtr) return;
    m_FreeByProto[cloneDesc.protoTag].push_back(objectPtr);
}

void CObjectPool::ClearAll()
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

void CObjectPool::ClearByProto(const CLONE_DESC& cloneDesc)
{
    auto it = m_FreeByProto.find(cloneDesc.protoTag);
    if (it == m_FreeByProto.end()) return;

    for (CGameObject* objectPtr : it->second)
        Safe_Release(objectPtr);

    it->second.clear();
    m_FreeByProto.erase(it);
}

CObjectPool* CObjectPool::Create()
{
    return new CObjectPool();
}

void CObjectPool::Free()
{
    ClearAll();
}
