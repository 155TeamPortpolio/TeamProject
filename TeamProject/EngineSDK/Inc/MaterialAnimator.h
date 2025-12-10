#pragma once
#include "Component.h"
NS_BEGIN(Engine)

class ENGINE_DLL CMaterialAnimator :
    public CComponent
{
public:
  
private:
    CMaterialAnimator();
    CMaterialAnimator(const CMaterialAnimator& rhs);
    virtual ~CMaterialAnimator() DEFAULT;
public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(COMPONENT_DESC* pArg) override;
    void Update_Animation(_float dt);

public:
    void LinkAnimate_Material(class CMaterial* pMaterial);
    HRESULT RegisterKeyFrame(const string& subsetKey,const string& keyframeKey, const MATERIAL_CLIP& clip);
    HRESULT Change_Animation(const string& subsetKey, const string& keyframeKey, _bool OverrideSame = false);

private:
    void Update_KeyFrame(const string& subsetKey, MAT_KEYFRAME& KeyFrame, _float dt);

private:
    /*마스터 머티리얼을 기준으로 "머티리얼을 찾고"*/
    class CMaterial* m_pMasterMaterial = { nullptr };
    unordered_map<string, vector<MAT_KEYFRAME>> m_Clips;
    unordered_map<string, _uint> m_ClipNames;
    unordered_map<string, string> m_NowAnimations;

public:
    static CMaterialAnimator* Create();
    virtual CComponent* Clone();
    void Free() override;
};

NS_END