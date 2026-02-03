#pragma once
#include "Model.h"
#include "ParticleParams.h"

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
		_float pad0{};

		_float4 vColor{};

		_float fLifeTime{};
		_float fMaxLifeTime{};
		_float2 vStartSize{};

		_float2 vSize{};
		_uint iFrameIndex{};
		_float fNoiseFrequency{};
	}PARTICLE_GPU;

	typedef struct tagInstanceData
	{
		_float4 vRight{}, vUp{}, vLook{};
		_float4 vTranslate{};

		_float3 vVelocity{};
		_float pad0{};

		_float4 vColor{};

		_float2 vLife{};
		_uint iFrameIndex{};
		_float pad1{};
	}INSTANCE_DATA;

	typedef struct tagCBFrame
	{
		_uint iModuleMask{};
		_float fDeltaTime{};
		_uint iAliveCount{};
		_uint iMaxParticles{};
		_uint UseGravity{};
		_float fGravityScale{};
		_float2 pad0{};

		/*Life Time Velocity*/
		_float fDampScale{};
		_float3 pad1{};

		/*Life Time Size*/
		_float2 vStartScale{};
		_float2 vEndScale{};

		/*Life Time Color*/
		_float4 vStartColor{};
		_float4 vEndColor{};
		
		/*Life Time Alpha*/
		_float4 vAlphaKey{};
		_float2 vRatio{};
		_float2 pad2{};

		/*Texture Sheet Animation*/
		_uint isAnimated{};
		_uint iMaxFrameIndex{};
		_float2 pad3{};

		/*Noise*/
		_float fElapsedTime{};
		_float3 pad4{};

		_float4 vStrength{};
		_float4 vFrequency{};
		_float4 vScrollSpeed{};
	}CB_FRAME;

	typedef struct tagCBPacked
	{
		_uint iInstanceCount{};
		_uint pad[3] = {};
	}CB_PACKED;
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

	enum class SHADER { SPAWN, BASIC, INIT_DEAD_LIST, BUILD, END };
	enum class PARTICLE_SPACE { LOCAL, WORLD, END };
	enum class SPAWN_SHAPE { SPHERE, BOX, CONE, END };
	enum class COLOR_MODE : _uint
	{
		MULTIPLY = 0 ,
		ADDITIVE = 1,
		END = 3
	};
	
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
	HRESULT Draw(ID3D11DeviceContext* pContext);
	_bool IsWorldSpace() { return (m_eParticleSpace == PARTICLE_SPACE::WORLD); };

public:
	void SetParticleParams(PARTICLE_NODE particleDesc);
	void Simulation_Particle(_float dt);
	void Reset();
	void Pause();

public:
	virtual void Render_GUI() override;

private:
	void ApplyPending();
	void CreateStructuredBuffers(_uint iMaxCount);
	void ReadAliveOutCount();

	void SpawnParticles(_float dt);
	void ResetAliveOut();
	void UploadSpawnIn();
	void UpdateParticles(_float dt);
	void BuildInstanceData();
	void SetUpParticle(PARTICLE_GPU& particle)const;

	class CVIBuffer* m_pPoint = { nullptr };

	_uint m_iMaxInstancesCount{};
	_bool isDrawing = { true };

	_bool m_IsPause = false;
	_bool m_IsChanged = false;
	PARTICLE_NODE m_PendingChanged{};

	/*Main Params*/
	_uint m_iRGBMaskMode{};
	PARTICLE_SPACE m_eParticleSpace = PARTICLE_SPACE::WORLD;
	MODULE_MASK m_eModuelMask{};
	COLOR_MODE m_eColorMode = COLOR_MODE::ADDITIVE;
	_float2 m_vPivot{ 0.5f,0.5f };
	_float3 m_vRimLightColor{};

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

	SPAWN_SHAPE m_eSpawnShape = SPAWN_SHAPE::BOX;
	_float3 m_vCenter{};
	_float3 m_vHalfBox{};
	_float m_fRadius{};

	/*Gravity mode*/
	_bool m_UseGravity = false;
	_float m_fGravityScale{};

	/*Modules*/
	LIFE_TIME_VELOCITY m_LifeTimeVelocity{};
	LIFE_TIME_SIZE m_LifeTimeSize{};
	LIFE_TIME_COLOR m_LifeTimeColor{};
	LIFE_TIME_ALPHA m_LifeTimeAlpha{};
	TEXTURE_SHEET_ANIMATION m_TextureSheetAnimation{};
	NOISE m_Noise{};

	/*------------------------------컴퓨트 셰이더 이식중---------------------------------*/
	class CStructuredBuffer* m_pInstanceBuffer = { nullptr };
	class CStructuredBuffer* m_pParticlesBuffer = { nullptr };
	class CStructuredBuffer* m_pDeadListBuffer = { nullptr };
	class CStructuredBuffer* m_pAliveBuffer[2] = {};
	class CStructuredBuffer* m_pSpawnInBuffer = { nullptr };
	
	vector<class CComputeShader*> m_ComputeShaders;
	ID3D11Buffer* m_pCBDeadListInitBuffer = { nullptr };
	ID3D11Buffer* m_pCBFrameBuffer = { nullptr };
	ID3D11Buffer* m_pCBSpawnBuffer = { nullptr };
	ID3D11Buffer* m_pCBPacked = { nullptr };
	ID3D11Buffer* m_pCounterGPU = { nullptr };
	ID3D11Buffer* m_pCounterStaging = { nullptr };

	_uint m_iAliveInIndex = 0;
	_uint m_iAliveOutIndex = 1;							//이번 프레임에 alive out으로 사용할 버퍼 인덱스
	_uint m_iAliveCount{};								//현재 살아있는 파티클 갯수
	vector<PARTICLE_GPU> m_SpawnList;

public:
	static CParticleSystem* Create();
	CComponent* Clone() override;
	virtual void Free() override;

};
NS_END
