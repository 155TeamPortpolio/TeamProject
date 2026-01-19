#pragma once

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CCursorController final
{
public:
    void Initialize();
    void Update(_float dt);

public:
    void SetMouseLock(_bool lock);
    void ToggleMouseLock();

private:
    void ToggleGui();
    void SyncMouseLockByGui();

private:
    RECT  GetClientRectInScreen() const;
    POINT GetClientCenterInScreen() const;

private:
    _bool m_isMouseLocked = false;
    _bool m_cachedGuiActive = true;
};

NS_END