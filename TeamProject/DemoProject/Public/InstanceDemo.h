#pragma once
#include "GameObject.h"
NS_BEGIN(Demo)

/*파티클 용 구조체 -> 클라이언트에서 원하는 구조체를 설정함*/
typedef struct  tagInstanceParticle
{
    _float4			vRight, vUp, vLook, vTranslation;
}INSTANCE_PARTICLE;

typedef struct   tagVertexParticleInstancing {
    static constexpr unsigned int					iElementCount = { 4 };
    static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[4] = {
        { "INSTANCE", 0,	DXGI_FORMAT_R32G32B32A32_FLOAT, 1,0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "INSTANCE", 1,	DXGI_FORMAT_R32G32B32A32_FLOAT, 1,16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "INSTANCE", 2,	DXGI_FORMAT_R32G32B32A32_FLOAT, 1,32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "INSTANCE", 3,	DXGI_FORMAT_R32G32B32A32_FLOAT, 1,48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    };
}VTX_PARTICLEINSTANCE;


class CInstanceDemo :
    public CGameObject
{
protected:
    CInstanceDemo();
    CInstanceDemo(const CInstanceDemo& rhs);
    virtual ~CInstanceDemo() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void Priority_Update(_float dt) override;
    void Update(_float dt) override;
    void Late_Update(_float dt) override;

public:
    void Render_GUI() override;

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };

private:
    _uint m_ParicleCount = {};
    vector< _float3 > m_vVelocities;
    vector<INSTANCE_PARTICLE> m_Particle;
    vector<INSTANCE_INIT_DESC> m_InitDescs;

public:
    static CInstanceDemo* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free();
};
NS_END
