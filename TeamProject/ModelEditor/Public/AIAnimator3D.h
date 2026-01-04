#pragma once
#include "ModelEditor_Defines.h"
#include "Animator3D.h"

NS_BEGIN(ModelEdit)
class CAIModelData;

class CAIAnimator3D final : public CAnimator3D
{
private:
    CAIAnimator3D();
    virtual ~CAIAnimator3D() DEFAULT;

public:
    HRESULT Initialize(const aiScene* scene, CAIModelData* modelData);
    virtual void Render_GUI() override;

public:
    HRESULT Save_Animation(const string& savePath, const _float4x4* worldMat);

public:
    static CAIAnimator3D* Create(const aiScene* scene, CAIModelData* modelData);
    virtual void Free() override;
};

NS_END