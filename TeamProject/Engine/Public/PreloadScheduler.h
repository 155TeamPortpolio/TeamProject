#pragma once
#include "Base.h"
#include "PreLoad_Struct.h"

NS_BEGIN(Engine)
class CPreloadScheduler :
    public CBase
{
private:
    CPreloadScheduler(class CThreadPool* pThreadPool);
    ~CPreloadScheduler();

public:
    _bool Request(const PreloadKey& requestKey);
    void Pump(vector<PreloadCompleted>& outCompleted);
    void GetProgress(_uint& outDone, _uint& outTotal);
    PreloadState GetState(const PreloadKey& requestKey) const;
    using LoaderFunc = function<bool(const PreloadKey&, string&)>;
    void BindLoader(ResourceType type, LoaderFunc loader);
    void Reset();

private:
    void SetState(const PreloadKey& requestKey, PreloadState state, const std::string& errorMessage);
    void SetError(const PreloadKey& requestKey, const std::string& errorMessage);
 
private:
    class CThreadPool* m_threadPool = {nullptr};
    mutable mutex m_mutex;
   unordered_map<PreloadKey, PreloadTask, PreloadKeyHash> m_tasks;
   atomic<_uint> m_done{ 0 };
   atomic<_uint> m_total{ 0 };

    mutable mutex m_loaderMutex;
   unordered_map<ResourceType, LoaderFunc> m_loaders;

public:
    static CPreloadScheduler* Create(class CThreadPool* pThreadPool);
    virtual void Free() override;
};
NS_END
