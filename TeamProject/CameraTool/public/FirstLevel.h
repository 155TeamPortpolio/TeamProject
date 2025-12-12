#pragma once

#include "Level.h"

NS_BEGIN(CameraTool)

class FirstLevel : public CLevel
{
private:
	FirstLevel(const string& key);
	
public:
    virtual HRESULT Initialize() override;
    virtual HRESULT Awake()      override;
    virtual void    Update()     override;
    virtual HRESULT Render()     override;

private:
    CGameInstance* game{};

public:
    static FirstLevel* Create(const string& key);
    virtual void Free() override;
};

NS_END