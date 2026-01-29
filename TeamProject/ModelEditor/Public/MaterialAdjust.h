#pragma once
#include "GameObject.h"

NS_BEGIN(ModelEdit)
class CMaterialAdjust :
    public CGameObject
{
protected:
    CMaterialAdjust();
    CMaterialAdjust(const CMaterialAdjust& rhs);
    virtual ~CMaterialAdjust() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

public:
    void Render_GUI() override;
private:
    void Render_AdjustTab();
    void Render_FileHierarchy();
    string ConvertToConstant(TEXTURE_TYPE eType);

public:
    MaterialFile Load_Materials(string path);
    HRESULT Save_Materials();

protected:
    _bool isTabOpen = { false };
    MaterialFile m_Files;
    string OriginPath = {};
    _int m_SelectedFile = -1;
    _int m_SelectedMaterial = -1;
    _int m_SelectedType = -1;
    _int m_SelectedTexture = -1;

public:
    static CMaterialAdjust* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();
};

NS_END