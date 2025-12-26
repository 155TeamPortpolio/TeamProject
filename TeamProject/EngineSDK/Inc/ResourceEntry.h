#pragma once
#include "Base.h"
#include <atomic>
#include <future>
#include <mutex>
#include <variant>
#include <string>

NS_BEGIN(Engine)

class CTexture;
class CModelData;
class CShader;
class CMaterialData;
class CAnimationClip;
class CSoundData;

class CResourceEntry : public CBase
{

private:
    CResourceEntry();
    ~CResourceEntry() DEFAULT;

public:
    enum class ResourceType : uint8_t { Texture, Model, Shader, Material, Audio, Animation, Unknown };
    enum class LoadState : uint8_t { Unloaded, Loading, Committing, Ready, Failed };
    struct ResourceKey {
        uint64_t levelHash{};
        ResourceType type{};
        uint64_t keyHash{};
        bool operator==(const ResourceKey& rhs) const {
            return levelHash == rhs.levelHash && type == rhs.type && keyHash == rhs.keyHash;
        }
    };
    struct ResourceKeyHash {
        size_t operator()(const ResourceKey& resourceKey) const noexcept {
            size_t combined = size_t(resourceKey.levelHash);
            combined ^= size_t(resourceKey.keyHash) + 0x9e3779b97f4a7c15ull + (combined << 6) + (combined >> 2);
            combined ^= (size_t(resourceKey.type) * 0x9e3779b97f4a7c15ull);
            return combined;
        }
    };

    using ResourceVariant = variant<
         monostate,
        CTexture*,
        CModelData*,
        CShader*,
        CMaterialData*,
        CAnimationClip*,
        CSoundData*
    >;

    // 엔트리 생존 범위 내에서 "잠깐 보기"(ref 증가 없음)
    template<typename TResource>
    TResource* Get_NoRef() const
    {
        lock_guard<mutex> lockGuard(m_Mutex);
        TResource* const* foundPointer = get_if<TResource*>(&m_Resource);
        return (foundPointer && *foundPointer) ? *foundPointer : nullptr;
    }

    // 외부가 소유해서 들고 가야 하면 Acquire (AddRef + 반환)
    template<typename TResource>
    TResource* Acquire() const
    {
        lock_guard<mutex> lockGuard(m_Mutex);
        TResource* const* foundPointer = get_if<TResource*>(&m_Resource);
        if (!foundPointer || !(*foundPointer)) return nullptr;
        Safe_AddRef(*foundPointer);
        return *foundPointer;
    }

    using LoaderFunc = function<ResourceVariant(const string& sourcePath, string& outError)>;
    using CommitFunc = function<bool(ResourceVariant& inOutVariant, string& outError)>;

public:
    
    bool Begin_LoadAsync(const LoaderFunc& loaderFunc);
    void Pump(const CommitFunc& commitFunc);  // 메인스레드에서 호출: Loading 완료되면 Committing으로 넘기고, Commit까지 끝나면 Ready/Failed 확정
    void Reset();

private:

    static void AddRefVariant_NoLock(ResourceVariant& variantValue)
    {
        if (auto** pointerValue = get_if<CTexture*>(&variantValue)) Safe_AddRef(*pointerValue);
        else if (auto** pointerValue = get_if<CModelData*>(&variantValue)) Safe_AddRef(*pointerValue);
        else if (auto** pointerValue = get_if<CShader*>(&variantValue)) Safe_AddRef(*pointerValue);
        else if (auto** pointerValue = get_if<CMaterialData*>(&variantValue)) Safe_AddRef(*pointerValue);
        else if (auto** pointerValue = get_if<CAnimationClip*>(&variantValue)) Safe_AddRef(*pointerValue);
        else if (auto** pointerValue = get_if<CSoundData*>(&variantValue)) Safe_AddRef(*pointerValue);
    }

    static void ReleaseVariant_NoLock(ResourceVariant& variantValue)
    {
        if (auto** pointerValue = get_if<CTexture*>(&variantValue)) Safe_Release(*pointerValue);
        else if (auto** pointerValue = get_if<CModelData*>(&variantValue)) Safe_Release(*pointerValue);
        else if (auto** pointerValue = get_if<CShader*>(&variantValue)) Safe_Release(*pointerValue);
        else if (auto** pointerValue = get_if<CMaterialData*>(&variantValue)) Safe_Release(*pointerValue);
        else if (auto** pointerValue = get_if<CAnimationClip*>(&variantValue)) Safe_Release(*pointerValue);
        else if (auto** pointerValue = get_if<CSoundData*>(&variantValue)) Safe_Release(*pointerValue);
        variantValue = std::monostate{};
    }

public:
    void SetKey(const ResourceKey& resourceKey) {
        lock_guard<mutex> lockGuard(m_Mutex);
        m_ID = resourceKey;
    }
    ResourceKey GetKey() const { lock_guard<mutex> lock(m_Mutex); return m_ID; }

    void SetSourcePath(const string& sourcePath) {
        lock_guard<mutex> lockGuard(m_Mutex);
        m_SourcePath = sourcePath;
    }

    LoadState GetState() const { return m_State.load(memory_order_acquire); }
    const string& GetLastError() const { return m_LastErrorMsg; }
    const string& GetDebugName() const { return m_DebugName; }
    void SetDebugName(const string& debugName) { m_DebugName = debugName; }
    private:
    void Release_NoLock();

private:
    ResourceKey m_ID{};
    string m_SourcePath;
    atomic<LoadState> m_State{ LoadState::Unloaded };

    mutable mutex m_Mutex;
    ResourceVariant m_Resource{monostate{} };

    // 로딩 결과 임시 보관(Commit 전)
    ResourceVariant m_StagingResource{ monostate{} };
   shared_future<ResourceVariant> m_LoadingTask;
   string m_LastErrorMsg;
   string m_DebugName;

public:
    static CResourceEntry* Create();
    virtual void Free() override;
};

NS_END
