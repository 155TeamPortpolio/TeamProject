#pragma once
#include "ILightService.h"
NS_BEGIN(Engine)
class CLightMgr :
    public ILightService
{
    enum class LightState {
        NONE = 0,           // 비어 있음 (슬롯 미사용)
        ACTIVE = 1,         // 사용 가능
        INACTIVE = 2,   // 일시 비활성 (제외)
        DEAD = 3,           // 소유자가 삭제됨, 완전히 제거 예정
    };

    struct LightSlot {
        _bool alive = false;
        _uint generation = {};
        class CLight* pLight = { nullptr };
        LightState eState = { LightState::NONE };
        LIGHT_DESC descSnapShot;
    };

private:
    CLightMgr();
    virtual ~CLightMgr();

public:
    HRESULT Initialize();
    virtual _int Register_Light(class CLight* Light, _int Index)override;
    virtual void UnRegister_Light(class CLight* Light, _int Index)override;
    virtual void DeActive_Light(class CLight* Light, _int Index)override;
    virtual void Active_Light(class CLight* Light, _int Index)override;
public:
    virtual vector<LIGHT_DESC> Visible_Lights() override;

private:
    void CleanUp(); // 매 프레임의 마지막에 빛을 정리해줌

private:
    vector<LightSlot> m_LightSlots;

public:
    static CLightMgr* Create();
    virtual void Free() override;
};

NS_END