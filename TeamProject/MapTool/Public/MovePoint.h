#pragma once
#include "MapToolObject.h"

NS_BEGIN(MapTool)
class CMovePoint :
    public CMapToolObject
{
private:
    CMovePoint();
    CMovePoint(const CMovePoint& rhs);
    virtual ~CMovePoint() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Awake() override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

    virtual void Export_ObjectData(void* pDesc) override;

    void Set_GUI(class CMapToolGui* pGui) {
        if (pGui) m_pGUi = pGui;
    }

    void Set_PathOrder(_int iPathIndex, _int iOrderIndex) { 
        m_iPathIndex = iPathIndex;
        m_iOrderIndex = iOrderIndex;
    }

    _int Get_PathIndex() {
        return m_iPathIndex;
    }

    _int Get_OrderIndex() { 
        return m_iOrderIndex;
    }
public:
    void Render_GUI() override;

private:
    class CMapToolGui* m_pGUi = { nullptr };
    _int m_iPathIndex = { -1 };
    _int m_iOrderIndex = { -1 };

public:
    static CMovePoint* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();
};
NS_END
