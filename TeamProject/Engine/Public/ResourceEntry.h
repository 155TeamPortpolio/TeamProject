#pragma once
#include "Base.h"
#include <atomic>
#include <future>
#include <mutex>
#include <variant>
#include <string>
#include "Resource_Blobs.h"

NS_BEGIN(Engine)
class ENGINE_DLL CResourceEntry : public CBase
{
private:
    CResourceEntry();
    ~CResourceEntry() DEFAULT;
public:
    template<typename TResource>
    TResource* Get_NoRef() const /*레퍼런스 카운트 증가없이 반환하는 함수*/
    {
        lock_guard<mutex> lockGuard(m_Mutex);

        if (TResource* const* foundPointer = get_if<TResource*>(&m_Resource))
            if (*foundPointer) return *foundPointer;

        if (TResource* const* fallbackPointer = get_if<TResource*>(&m_FallbackResource))
            return (fallbackPointer && *fallbackPointer) ? *fallbackPointer : nullptr;

        return nullptr;
    }

    template<typename TResource>
    TResource* Acquire() const /*레퍼런스 카운트 있이 반환하는 함수*/
    {
        lock_guard<mutex> lockGuard(m_Mutex);

        TResource* result = nullptr;

        if (TResource* const* foundPointer = get_if<TResource*>(&m_Resource))
            result = (foundPointer && *foundPointer) ? *foundPointer : nullptr;

        if (!result)
        {
            if (TResource* const* fallbackPointer = get_if<TResource*>(&m_FallbackResource))
                result = (fallbackPointer && *fallbackPointer) ? *fallbackPointer : nullptr;
        }

        if (!result) return nullptr;

        Safe_AddRef(result);
        return result;
    }
    bool Begin_LoadAsync(const LoaderFunc& loaderFunc, const ScheduleFunc& scheduleFunc);
    void Pump_CompletedOnly(); // 메인 스레드 전용
    void Pump();
    void Reset();
public:
    void ForceSetReady(ResourceVariant resourceVariant);
    void SetFallback(ResourceVariant fallbackVariant);
 
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
    _uint GetGenerationCopy() const
    {
        lock_guard<mutex> lockGuard(m_Mutex);
        return m_Generation;
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
    ResourceVariant m_FallbackResource{ std::monostate{} }; // 디폴트
    shared_future<ResourceVariant> m_LoadingTask;

    // ===== Error / Control =====
    string m_LastErrorMsg;
    _uint m_Generation = 0; // Reset 중 결과 무효화용(쓰면 cpp에서도 체크)


    static void ReleaseVariant_NoLock(ResourceVariant& variantValue)
    {
        std::visit([](auto& heldValue)
            {
                using HeldType = std::decay_t<decltype(heldValue)>;
                if constexpr (!std::is_same_v<HeldType, std::monostate>)
                {
                    if (heldValue) Safe_Release(heldValue);
                }
            }, variantValue);

        variantValue = std::monostate{};
    }

    static void AddRefVariant_NoLock(ResourceVariant& variantValue)
    {
        std::visit([](auto& heldValue)
            {
                using HeldType = std::decay_t<decltype(heldValue)>;
                if constexpr (!std::is_same_v<HeldType, std::monostate>)
                {
                    if (heldValue) Safe_AddRef(heldValue);
                }
            }, variantValue);
    }

    public:
        static CResourceEntry* Create();
        virtual void Free();
};


NS_END
