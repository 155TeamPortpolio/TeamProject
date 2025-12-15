#pragma once
#include "Model.h"

NS_BEGIN(Engine)
class ENGINE_DLL CParticleSystem :
    public CModel
{
public:
	typedef struct tagParticle
	{
		_bool isAlive = false;
		_float fMaxLifeTime{};
		_float fLifeTime{};
		_float3 vVelocity{};
		_float3 vPosition{};
		_float4 vColor{};

		_float2 vStartSize{};
		_float2 vSize{};

		_uint iFrameIndex{};
	}PARTICLE;

	enum class PARTICLE_SPACE { LOCAL, WORLD, END };
protected:
	CParticleSystem();
	CParticleSystem(const CParticleSystem& rhs);
	virtual ~CParticleSystem() DEFAULT;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(COMPONENT_DESC* pArg) override;
	virtual const D3D11_INPUT_ELEMENT_DESC* Get_ElementDesc(_uint DrawIndex)override;
	virtual const _uint Get_ElementCount(_uint DrawIndex)override;
	virtual const string_view Get_ElementKey(_uint DrawIndex)override;
	virtual HRESULT Draw(ID3D11DeviceContext* pContext, _uint Index)override { return S_OK; }; /*파티클은 자신만의 Draw함수 따로 호출*/
	virtual HRESULT Link_Model(const string& levelKey, const string& modelDataKey)override;

public:
	_uint Get_MeshCount() override;
	_uint Get_MaterialIndex(_uint Index) override;
	_bool isDrawable(_uint Index) override;
	virtual void SetDrawable(_uint Index, _bool isDraw) override;
	virtual MINMAX_BOX Get_LocalBoundingBox()	override;
	virtual MINMAX_BOX Get_WorldBoundingBox()	override;
	virtual vector<MINMAX_BOX> Get_MeshBoundingBoxes()	override;
	virtual MINMAX_BOX Get_MeshBoundingBox(_uint index)	override;
	_bool isReadyToDraw()	override { return true; };

public:
	void SetParticleParams(PARTICLE_NODE particleDesc);
	void Simulation_Particle(_float dt);
	HRESULT Draw(ID3D11DeviceContext* pContext, _uint offset, _uint count);
	const std::vector<VTX_INSTANCE_POINT>& GetInstanceDatas() { return m_InstanceDatas; }

	_bool IsWorldSpace() { return (m_eParticleSpace == PARTICLE_SPACE::WORLD); };
public:
	virtual void Render_GUI() override;

private:
	void SpawnParticles(_float dt);
	void UpdateParticles(_float dt);
	void SetUpParticle(PARTICLE & particle)const;
	void BuildInstanceData();

	class CVIBuffer* m_pInstancePoint = { nullptr };
	_uint m_iMaxInstancesCount{};
	_bool isDrawing = { true };

	vector<PARTICLE> m_Particles;
	vector<VTX_INSTANCE_POINT> m_InstanceDatas;
	vector<_uint> m_DeadParticleIndices;

	/*Main Params*/
	PARTICLE_SPACE m_eParticleSpace = PARTICLE_SPACE::WORLD;
	_bool m_IsLoop = false;
	_uint m_iBurstCount{};

	_float m_fSpawnPerSec{};
	_float m_fSpawnAcc{};
	_uint m_iSpawnParticleCount{};
	_uint m_iMaxSpawnParticleCount{};

	_float2 m_vStartSpeed{};
	_float2 m_vStartLifeTime{};
	_float2 m_vStartSize{};
	_float3 m_vSpawnAreaMin{};
	_float3 m_vSpawnAreaMax{};
	
	/*Gravity mode*/
	_bool m_UseGravity = false;
	_float m_fGravityScale{};

	/*Modules*/
	_uint m_iTextureCol{ 1 };
	_uint m_iTextureRow{ 1 };

	class CLifeTimeVelocity* m_pLifeTimeVelocity = { nullptr };
	class CLifeTimeSize* m_pLifeTimeSize = { nullptr };
	class CLifeTimeColor* m_pLifeTimeColor = { nullptr };
	class CTextureSheetAnimation* m_pTextureSheetAnimation = { nullptr };
	vector<class IParticleModule*> m_Modules;

public:
	static CParticleSystem* Create();
	CComponent* Clone() override;
	virtual void Free() override;

};
NS_END
