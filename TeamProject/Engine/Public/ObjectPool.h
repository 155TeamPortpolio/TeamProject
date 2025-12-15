#pragma once
#include "Base.h"
class CObjectPool :
    public CBase
{
private:
    CObjectPool();
    ~CObjectPool() DEFAULT;

public:
    void Check_Alive();

private:
    unordered_map<string, vector<class CGamObject*>> m_ObjectPool;

public:
	static CObjectPool* Create();
	virtual void Free() override;
};