#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class CObjectPool :
    public CBase
{
private:
    CObjectPool();
    ~CObjectPool() DEFAULT;

public:
    class CGameObject* Acquire(const CLONE_DESC& cloneDesc);
    void Return(const CLONE_DESC& cloneDesc, CGameObject* objectPtr);

    void ClearAll();                 // 레벨 전환 등에서 전체 비우기
    void ClearByProto(const CLONE_DESC& cloneDesc);

private:
    unordered_map<string, vector<class CGameObject*>> m_FreeByProto;

public:
	static CObjectPool* Create();
	virtual void Free() override;
};

NS_END