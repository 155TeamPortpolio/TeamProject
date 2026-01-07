#pragma once

#include "Level.h"

NS_BEGIN(CameraTool)

class CFirstLevel final : public CLevel
{
private:
    CFirstLevel(const string& key) : CLevel(key) {}
    virtual ~CFirstLevel() DEFAULT;

public:
    virtual HRESULT Initialize() override { return S_OK; }
    virtual HRESULT Awake()      override;
    virtual void    Update()     override {}
    virtual HRESULT Render()     override { return S_OK; }

public:
    static CFirstLevel* Create(const string& key);
    virtual void Free() override { __super::Free(); }
};

NS_END