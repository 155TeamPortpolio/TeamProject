 #pragma once
#include "Model.h"
NS_BEGIN(Engine)
class ENGINE_DLL CStaticModel :
    public CModel
{
protected:
    CStaticModel();
    CStaticModel(const CStaticModel& rhs);
    virtual ~CStaticModel() DEFAULT;

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

    class CModelData* Get_ModelData() { return m_pData; };
    virtual MINMAX_BOX Get_LocalBoundingBox()	override;
    virtual MINMAX_BOX Get_WorldBoundingBox()	override;
    virtual vector<MINMAX_BOX> Get_MeshBoundingBoxes()	override;
    virtual MINMAX_BOX Get_MeshBoundingBox(_uint index)	override;
    _bool isReadyToDraw()	override { return m_pData != nullptr; };
    void Hide_MehsByName(const string& name);
    _int Get_MeshIndexByName(const string& name);
public:
    void Render_GUI();

protected:
    vector<_bool> m_DrawableMeshes;
    class CModelData* m_pData = { nullptr };

public:
    static CStaticModel* Create();
    virtual CComponent* Clone() override;
    virtual void Free() override;
};

NS_END