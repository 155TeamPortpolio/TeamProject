#pragma once
#include "Renderer.h"

NS_BEGIN(Engine)

class CSkinnedMeshRenderer  final:
    public CRenderer
{
private:
    CSkinnedMeshRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~CSkinnedMeshRenderer();
    HRESULT Initialize(class CTarget_Manager* pTargetManager, class CPipeLine* pPipeLine);

public:
    void Set_RimLightMode(RIMLIGHT eMode) { RimLightMode = eMode; }

public:
    HRESULT Render_SkinnedMesh(class SkinnedOpaquePass* pOpaquePass);
    HRESULT Render_RimLight();
    HRESULT Process_OutLineQueue();
    void Add_OutLineCommand(const OUTLINE_COMMAND& command);
public:
    void Update(_float dt);

private:
    virtual HRESULT Ready_Target() override;
    virtual HRESULT Ready_MRT() override;

private:
    RIMLIGHT RimLightMode = RIMLIGHT::OUTLINE;
    vector<OUTLINE_COMMAND> m_OutLineCommands;

public:
    static CSkinnedMeshRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
        CTarget_Manager* pTargetManager, CPipeLine* pPipeLine);
    virtual void Free() override;
};

NS_END