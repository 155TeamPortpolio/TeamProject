#pragma once
#include "Base.h"
#include <atomic>
#include <future>
#include <mutex>
#include <variant>
#include <string>
#include "Resource_Blobs.h"

NS_BEGIN(Engine)
class CResourceEntry : public CBase
{
private:
    CResourceEntry();
    ~CResourceEntry() DEFAULT;
public:
    template<typename TResource>
    TResource* Get_NoRef() const /*레퍼런스 카운트 증가없이 반환하는 함수*/
    {
        lock_guard<mutex> lockGuard(m_Mutex);
        TResource* const* foundPointer = get_if<TResource*>(&m_Resource);
        return (foundPointer && *foundPointer) ? *foundPointer : nullptr;
    }

    template<typename TResource>
    TResource* Acquire() const /*레퍼런스 카운트 있이 반환하는 함수*/
    {
        lock_guard<mutex> lockGuard(m_Mutex);
        TResource* const* foundPointer = get_if<TResource*>(&m_Resource);
        if (!foundPointer || !(*foundPointer)) return nullptr;
        Safe_AddRef(*foundPointer);
        return *foundPointer;
    }
    bool Begin_LoadAsync(const LoaderFunc& loaderFunc, const ScheduleFunc& scheduleFunc);
    void Pump_CompletedOnly(); // 메인 스레드 전용
    void Pump();
    void Reset();

public:
    LoadState GetState() const { return m_State.load(memory_order_acquire); }
    void SetKey(const ResourceKey& resourceKey)
    {
        lock_guard<mutex> lockGuard(m_Mutex);
        m_ID = resourceKey;
    }

    void SetSourcePath(const string& sourcePath)
    {
        lock_guard<mutex> lockGuard(m_Mutex);
        m_SourcePath = sourcePath;
    }

    void SetLevelTag(const string& levelTag)
    {
        lock_guard<mutex> lockGuard(m_Mutex);
        m_LevelTag = levelTag;
    }

    void SetDebugName(const string& debugName)
    {
        lock_guard<mutex> lockGuard(m_Mutex);
        m_DebugName = debugName;
    }

    void SetLastError(const string& errorMessage)
    {
        lock_guard<mutex> lockGuard(m_Mutex);
        m_LastErrorMsg = errorMessage;
    }

    // ===== Getters (Copy) =====
    ResourceKey GetKeyCopy() const
    {
        lock_guard<mutex> lockGuard(m_Mutex);
        return m_ID;
    }

    string GetSourcePathCopy() const
    {
        lock_guard<mutex> lockGuard(m_Mutex);
        return m_SourcePath;
    }

    string GetLevelTagCopy() const
    {
        lock_guard<mutex> lockGuard(m_Mutex);
        return m_LevelTag;
    }

    string GetDebugNameCopy() const
    {
        lock_guard<mutex> lockGuard(m_Mutex);
        return m_DebugName;
    }

    string GetLastErrorCopy() const
    {
        lock_guard<mutex> lockGuard(m_Mutex);
        return m_LastErrorMsg;
    }
    // ===== Meta / Identification =====
    ResourceKey  m_ID{};        // SetKey/GetKeyCopy에서 사용
    string  m_SourcePath;  // SetSourcePath/GetSourcePathCopy에서 사용
    string  m_LevelTag;    // SetLevelTag/GetLevelTagCopy에서 사용
    string  m_DebugName;   // SetDebugName/GetDebugNameCopy에서 사용

    // ===== State / Sync =====
    atomic<LoadState> m_State{ LoadState::Unloaded };
    mutable mutex     m_Mutex;

    // ===== Resource Payload =====
    ResourceVariant m_Resource{ monostate{} };
    shared_future<ResourceVariant> m_LoadingTask;

    // ===== Error / Control =====
    string m_LastErrorMsg;
    _uint m_Generation = 0; // Reset 중 결과 무효화용(쓰면 cpp에서도 체크)


    static void ReleaseVariant_NoLock(ResourceVariant& variantValue)
    {
        if (auto** pointerValue = get_if<CTexture*>(&variantValue)) Safe_Release(*pointerValue);
        else if (auto** pointerValue = get_if<CModelData*>(&variantValue)) Safe_Release(*pointerValue);
        else if (auto** pointerValue = get_if<CShader*>(&variantValue)) Safe_Release(*pointerValue);
        else if (auto** pointerValue = get_if<CMaterialData*>(&variantValue)) Safe_Release(*pointerValue);
        else if (auto** pointerValue = get_if<CAnimationClip*>(&variantValue)) Safe_Release(*pointerValue);
        else if (auto** pointerValue = get_if<CSoundData*>(&variantValue)) Safe_Release(*pointerValue);
        variantValue = monostate{};
    }

    public:
        static CResourceEntry* Create();
        virtual void Free();
};


NS_END
