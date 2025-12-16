#pragma once
#include "IParticleModule.h"

NS_BEGIN(Engine)
class CTextureSheetAnimation :
    public IParticleModule
{
public:
    typedef struct tagTextureSheetAnimationDesc : public IParticleModule::PARTICLE_MODULE_DESC
    {
        _bool isRandomFrameIndex = false;
        _bool isParticleAnimated = false;
        _uint iCol{ 1 };
        _uint iRow{ 1 };
        _uint iMaxFrameIndex{};
    }TEXTURE_SHEET_ANIMATION_DESC;
private:
    CTextureSheetAnimation();
    virtual ~CTextureSheetAnimation() DEFAULT;

public:
    void SetUpParticle(CParticleSystem::PARTICLE& particle);
    void SetParams(PARTICLE_MODULE_DESC* pDesc) override;
    void Update(CParticleSystem::PARTICLE& particle, _float dt) override;

public:
    static CTextureSheetAnimation* Create();
    virtual void Free() override;

private:
    _bool m_IsRandomFrameIndex = false;
    _bool m_IsParticleAnimated = false;
    _float m_fFrameSpeed{};
    _uint m_iCol{ 1 };
    _uint m_iRow{ 1 };
    _uint m_iMaxFrameIndex{};

};
NS_END
