#pragma once
#include "IParticleModule.h"

NS_BEGIN(Engine)
class CTextureSheetAnimation :
    public IParticleModule
{
public:
    typedef struct tagTextureSheetAnimationDesc : public IParticleModule::PARTICLE_MODULE_DESC
    {
        _float fFrameSpeed{};
        _uint iCol{ 1 };
        _uint iRow{ 1 };
    }TEXTURE_SHEET_ANIMATION_DESC;
private:
    CTextureSheetAnimation();
    virtual ~CTextureSheetAnimation() DEFAULT;

public:
    void SetParams(PARTICLE_MODULE_DESC* pDesc) override;
    void Update(CParticleSystem::PARTICLE& particle, _float dt) override;

public:
    static CTextureSheetAnimation* Create();
    virtual void Free() override;

private:
    _float m_fFrameSpeed{};
    _uint m_iCol{ 1 };
    _uint m_iRow{ 1 };

};
NS_END
