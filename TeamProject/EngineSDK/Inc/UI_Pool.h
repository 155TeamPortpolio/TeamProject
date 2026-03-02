#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class CUI_Pool :
    public CBase
{
private:
    CUI_Pool();
    ~CUI_Pool() DEFAULT;

public:
    class CUI_Object* Acquire(const CLONE_DESC& cloneDesc, INIT_DESC* pArg, _bool& outFirst);
    void Return(const CLONE_DESC& cloneDesc, CUI_Object* objectPtr);

    void ClearAll();                 // 레벨 전환 등에서 전체 비우기
    void ClearByProto(const CLONE_DESC& cloneDesc);

private:
    unordered_map<string, vector<class CUI_Object*>> m_FreeByProto;

public:
    static CUI_Pool* Create();
    virtual void Free() override;
};

NS_END