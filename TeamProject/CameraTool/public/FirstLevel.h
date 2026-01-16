#pragma once

#include "Level.h"

NS_BEGIN(CameraTool)

class CFirstLevel final : public CLevel
{
private:
    CFirstLevel(const string& key) : CLevel(key) {}
    virtual ~CFirstLevel() DEFAULT;

public:
    HRESULT Initialize() override { return S_OK; }
    HRESULT Awake()      override;
    void    Update()     override;
    HRESULT Render()     override { return S_OK; }

public:
    static CFirstLevel* Create(const string& key);
    void Free() override { __super::Free(); }
};

NS_END