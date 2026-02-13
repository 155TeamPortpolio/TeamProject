#pragma once
#include "Model.h"
NS_BEGIN(Engine)
class ENGINE_DLL CTessellationModel :
    public CModel
{
protected:
    CTessellationModel();
    CTessellationModel(const CTessellationModel& rhs);
    virtual ~CTessellationModel() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(COMPONENT_DESC* pArg) override;

public:
    virtual HRESULT Link_Model(const string& levelKey, const string& modelDataKey);
    HRESULT Draw(ID3D11DeviceContext* pContext, _uint Index) override;

public:
    virtual const D3D11_INPUT_ELEMENT_DESC* Get_ElementDesc(_uint DrawIndex) override;
    virtual const _uint Get_ElementCount(_uint DrawIndex) override;
    virtual const string_view Get_ElementKey(_uint DrawIndex) override;
    virtual _uint Get_MeshCount() override;
    virtual _uint Get_MaterialIndex(_uint Index) override;
    virtual _bool isDrawable(_uint Index) override;
    virtual void SetDrawable(_uint Index, _bool isDraw) override;

    virtual MINMAX_BOX Get_LocalBoundingBox()	override;
    virtual MINMAX_BOX Get_WorldBoundingBox()	override;
    virtual vector<MINMAX_BOX> Get_MeshBoundingBoxes()	override;
    virtual MINMAX_BOX Get_MeshBoundingBox(_uint index)	override;
    _bool isReadyToDraw()	override { return true; };

public:
    void Render_GUI();

protected:
    class CVI_Tessellation* m_pTessBuffer = nullptr;
    _float              m_fSize = 0.f;
    _uint               m_iGridSize = 0;
    _bool               isDrawing = true;

public:
    static CTessellationModel* Create(_uint iGridSize = 128, _float fTotalSize = 500.f);
    virtual CComponent* Clone() override;
    virtual void Free() override;
};
NS_END
