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
		_float fNoiseFrequency{};
	}PARTICLE;

	typedef struct tagParticle_GPU
	{
		_uint IsAlive{};
		_float3 vPosition{};
		_float3 vVelocity{};
		_float4 vColor{};
		_float fLifeTime{};
		_float fMaxLifeTime{};
		_float2 vStartSize{};
		_float2 vSize{};
		_float fNoiseFrequency{};
		_float2 pad{};
	}PARTICLE_GPU;

	typedef struct tagCBFrame
	{
		_float fDeltaTime{};
		_uint iAliveCount{};
		_uint iMaxParticles{};
		_uint UseGravity{};
		_float fGravityScale{};
		_float3 pad{};
	}CB_FRAME;

	typedef struct tagCBSpawn
	{
		_uint iSpawnCount{};
		_uint pad[3] = {};
	}CB_SPAWN;

	typedef struct tagCBDeadListInit
	{
		_uint iMaxParticleCount{};
		_uint pad[3] = {};
	}CB_DEAD_LIST_INIT;
	enum class SHADER { SPAWN, BASIC, INIT_DEAD_LIST, END };
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
	HRESULT Bind_Buffer(ID3D11DeviceContext* pContext);
	HRESULT Draw(ID3D11DeviceContext* pContext, _uint offset, _uint count);
	const std::vector<VTX_INSTANCE_POINT>& GetInstanceDatas() { return m_InstanceDatas; }
	_bool IsWorldSpace() { return (m_eParticleSpace == PARTICLE_SPACE::WORLD); };

public:
	void SetParticleParams(PARTICLE_NODE particleDesc);
	void Simulation_Particle(_float dt);

public:
	virtual void Render_GUI() override;

private:
	void CreateStructuredBuffers(_uint iMaxCount);
	void ReadAliveOutCount();

	void SpawnParticles(_float dt);
	void ResetAliveOut();
	void UploadSpawnIn();
	void UpdateParticles(_float dt);
	void BuildInstanceData();
	void SetUpParticle(PARTICLE_GPU& particle)const;

	class CVIBuffer* m_pPoint = { nullptr };
	ID3D11Buffer* m_pInstanceBuffer = { nullptr };

	_uint m_iMaxInstancesCount{};
	_bool isDrawing = { true };

	vector<PARTICLE> m_Particles;
	vector<VTX_INSTANCE_POINT> m_InstanceDatas;
	vector<_uint> m_DeadParticleIndices;

	/*Main Params*/
	PARTICLE_SPACE m_eParticleSpace = PARTICLE_SPACE::WORLD;

	_float m_fDelayDuration{};
	_float m_fElapsedTime{};

	_bool m_IsLoop = false;
	_uint m_iBurstCount{};

	_float m_fSpawnPerSec{};
	_float m_fSpawnAcc{};
	_uint m_iSpawnParticleCount{};		//현재까지 방출한 파티클 갯수
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
	class CNoise* m_pNoise = { nullptr };
	vector<class IParticleModule*> m_Modules;

	/*------------------------------컴퓨트 셰이더 이식중---------------------------------*/
	class CStructuredBuffer* m_pParticlesBuffer = { nullptr };
	class CStructuredBuffer* m_pDeadListBuffer = { nullptr };
	class CStructuredBuffer* m_pAliveBuffer[2] = {};
	class CStructuredBuffer* m_pSpawnInBuffer = { nullptr };
	
	vector<class CComputeShader*> m_ComputeShaders;
	ID3D11Buffer* m_pCBDeadListInitBuffer = { nullptr };
	ID3D11Buffer* m_pCBFrameBuffer = { nullptr };
	ID3D11Buffer* m_pCBSpawnBuffer = { nullptr };
	ID3D11Buffer* m_pCounterGPU = { nullptr };
	ID3D11Buffer* m_pCounterStaging = { nullptr };

	_uint m_iAliveInIndex = 0;
	_uint m_iAliveOutIndex = 1;							//이번 프레임에 alive out으로 사용할 버퍼 인덱스
	_uint m_iAliveCount{};								//현재 살아있는 파티클 갯수
	vector<PARTICLE_GPU> m_SpawnList;

	_uint m_iBaseOffset{};

public:
	static CParticleSystem* Create();
	CComponent* Clone() override;
	virtual void Free() override;

};
NS_END
