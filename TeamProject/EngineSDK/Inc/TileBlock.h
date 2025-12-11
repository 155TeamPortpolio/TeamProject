#pragma once
#include "Component.h"
NS_BEGIN(Engine)

class ENGINE_DLL CTileBlock :
    public CComponent
{
private:
    CTileBlock();
    CTileBlock(const CTileBlock& rhs);
    virtual ~CTileBlock() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(COMPONENT_DESC* pArg) override;

public:
    void Update_Position( TILESYSTEM_INFO& systemInfo);
    _uint Get_NeigborState();
    _uint Get_NeigborCount();

public:
    TILE_INDEX On_Grid(TILE_INDEX tileIndex, string tileType, _bool CanFail = false);

    void Set_Index(TILE_INDEX tileIndex);
    TILE_INDEX Get_Index() { return m_tIndex; };

private:
    class CTransform* m_pTransform = { nullptr };
    class ITileService* m_pTileSystem = { nullptr };

    TILE_INDEX m_tIndex = {};
    string m_eType = {};

public:
    static CTileBlock* Create();
    virtual CComponent* Clone() override;
    virtual void Free() override;
};

NS_END

