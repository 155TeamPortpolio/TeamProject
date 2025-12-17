#pragma once
#include "ICameraService.h"
#include "Camera.h"

NS_BEGIN(Engine)

class CCameraMgr : public ICameraService
{
private:
    CCameraMgr();
    virtual ~CCameraMgr() DEFAULT;
public:
    virtual void Set_MainCam(CCamera* camComp) override;
    virtual void Set_ShadowCam(CCamera* camComp) override;

public:
    virtual void Update(_float dt) override;

    virtual const Matrix* Get_ViewMatrix()               override { return &view;    }
    virtual const Matrix* Get_ProjMatrix()               override { return &proj;    }
    virtual const Matrix* Get_InversedViewMatrix()       override { return &invView; }
    virtual const Matrix* Get_InversedProjMatrix()       override { return &invProj; }
    virtual const _float4 Get_CameraPos()                override { return camPos;   }
    virtual const _float  Get_Far()                      override { return mainCam ? mainCam->Get_Far() : 0.f; }

    virtual const Matrix* Get_ShadowViewMatrix()         override { return &shadowView;    }
    virtual const Matrix* Get_ShadowProjMatrix()         override { return &shadowProj;    }
    virtual const Matrix* Get_InversedShadowViewMatrix() override { return &shadowInvView; }
    virtual const Matrix* Get_InversedShadowProjMatrix() override { return &shadowInvProj; }
    virtual const _float4 Get_ShadowCameraPos()          override { return shadowCamPos;   }
    virtual const _float  Get_ShadowFar()                override { return shadowCam ? shadowCam->Get_Far() : 0.f; }

public:
    static CCameraMgr* Create() { return new CCameraMgr(); }
    virtual void Free() override;

private:
    CCamera* mainCam{};
    CCamera* shadowCam{};

    Matrix   view{};
    Matrix   proj{};
    Matrix   invView{};
    Matrix   invProj{};
    Vector4  camPos{};

    Vector4  shadowCamPos{};
    Matrix   shadowView{};
    Matrix   shadowProj{};
    Matrix   shadowInvView{};
    Matrix   shadowInvProj{};
};

NS_END