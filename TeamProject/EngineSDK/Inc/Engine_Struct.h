#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{
	typedef struct tagEngineDesc {
		HWND hWnd;
		WINMODE			eWinMode;
		unsigned int	iWinSizeX, iWinSizeY;
	}ENGINE_DESC;

	/* Key Input struct*/
	typedef struct tagKeyDesc {
		_bool PrevDown = false;
		_bool CurrDown = false;
		KEY_STATE state = KEY_STATE::NONE_KEY;
	}KEY_DESC;

	typedef struct  tagMouseDesc
	{
		KEY_DESC mouseKey[3]; //0 L 1R 2M
		_float fDeltaX;
		_float fDeltaY;
		_float fWheelDelta;
	}MOUSE_DESC;

	/* Init Desc struct*/
	typedef struct tagInitDesc {
		virtual ~tagInitDesc() DEFAULT;
	}INIT_DESC;

	/* Light Desc struct*/
	typedef struct tagLightDesc {
		union { _float4 vLightPosition;  _float4 vOffsetPosition; };
		_float4		vLightDirection = { 0,-1,0,0 };
		_float4		vLightDiffuse = {};
		_float4		vLightAmbient = {};
		_float4		vLightSpecular = {};
		_float			fLightRange = {};
		_float			fLightIntensity = { 3.f };
		_float2		lightPadding = {};
		LIGHT_TYPE eType = { LIGHT_TYPE::DIRECTIONAL };
	}LIGHT_DESC;

	/*File Info Desc*/
	/*Model*/
	typedef struct ENGINE_DLL tagModelFileHeader {
		char ModelKey[MAX_PATH];
		_uint MeshCount = {};
		_bool isAnimate = { false };
	}MODEL_FILE_HEADER;

	typedef struct ENGINE_DLL tagMeshInfoHeader {
		char MeshName[MAX_PATH];
		_uint VerticesCount = {};
		_uint IndicesCount = {};
		_uint MaterialIndex = {};
		_uint BoneCount = {};
		_uint offsetCount = {};
	}MESH_INFO_HEADER;

	typedef struct ENGINE_DLL tagMeshOffsetHeader {
		_uint BoneIndex;
		_float4x4 offsetMat;
	}MESH_OFFSET;

	typedef struct ENGINE_DLL tagSkeletonFileHeader {
		_uint BoneCount = {};
	}SKELETON_FILE_HEADER;

	typedef struct ENGINE_DLL tagBoneInfoHeader {
		char BoneName[MAX_PATH];
		_int ParentBoneIndex = {};
		_float4x4 TransformationMatrix = {};
	}BONE_INFO_HEADER;

	/*Material*/
	struct MaterialConstants
	{
		_float4 vMtrDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
		_float4 vMtrlAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
		_float4 vMtrlSpecular = _float4(1.0f, 1.0f, 1.0f, 1.f);
		_float4 vEmissive = _float4(0.f, 0.f, 0.f, 0.f);
		_float fSpecularPow = { 0.1f };
		_float3 vPadding;
	};

	struct BloomConstants
	{
		_float BloomType = 0;
		_float BloomIntensity = 1.f;
		_float2 Center = _float2(0.5f, 0.5f);
	};

	typedef struct ENGINE_DLL tagMaterialFileHeader {
		_uint MaterialDataCount = {};
		char materialFileKey[MAX_PATH];
	}MATERIAL_FILE_HEADER;

	typedef struct ENGINE_DLL tagMaterialInfoHeader {
		_uint TextureTypeCount = {};
		char materialDataKey[MAX_PATH];
		char ShaderKey[MAX_PATH];
		char passConstant[MAX_PATH];
		MaterialConstants materialConstant = {};
	}MATERIAL_INFO_HEADER;

	typedef struct ENGINE_DLL tagTextuerFileHeader {
		_uint typeID = {};
		_uint TextureCount = {};
	}TEXTURE_FILE_HEADER;

	typedef struct ENGINE_DLL tagTextuerInfoHeader {
		char TextureKey[MAX_PATH];
	}TEXTURE_INFO_HEADER;

	/*Animation*/
	typedef struct ENGINE_DLL tagAnimationInfoHeader {
		_float					fDuration = {};
		_float					fTickPerSecond = {};
		_uint					iNumChannels = {};
		char					ClipName[MAX_PATH];
	}ANIMATION_CLIP_HEADER;

	typedef struct ENGINE_DLL tagAnimationChannelHeader {
		_bool				isRootBoneChannel = { false };
		_uint				iBoneIndex = {};
		_uint				iNumKeyFrames = {};
		char				BoneName[MAX_PATH];
	}ANIMATION_CHANNEL_HEADER;

	struct _XMKeyFrame {
		_vector vScale;
		_vector vRotation;
		_vector vTranslation;
	};

	typedef struct ENGINE_DLL tagKeyFrame
	{
		_float3			vScale;
		_float4			vRotation = { 0,0,0,1 };
		_float3			vTranslation;
		_float			fTrackPosition;

		_bool IsBefore(_float nowTrackPosition) {
			return fTrackPosition < nowTrackPosition;
		}

		_XMKeyFrame LerpKeyFram(const tagKeyFrame& nextFrame, _float nowTrackPosition) {
			_XMKeyFrame lerpedFrame = {};
			_float fRatio = (nowTrackPosition - fTrackPosition) / (nextFrame.fTrackPosition - fTrackPosition);
			_float4 nextRot = nextFrame.vRotation;

			_vector nextRotation = XMLoadFloat4(&nextRot);
			_vector nowRotation = XMLoadFloat4(&vRotation);

			if (XMVector4Equal(nextRotation, XMVectorZero()))
				nextRotation = XMQuaternionIdentity();
			if (XMVector4Equal(nowRotation, XMVectorZero()))
				nowRotation = XMQuaternionIdentity();

			lerpedFrame.vScale = XMVectorLerp(XMLoadFloat3(&vScale), XMLoadFloat3(&nextFrame.vScale), fRatio);
			lerpedFrame.vRotation = XMQuaternionSlerp(nowRotation, nextRotation, (float)fRatio);
			lerpedFrame.vTranslation = XMVectorLerp(XMVectorSetW(XMLoadFloat3(&vTranslation), 1.f), XMVectorSetW(XMLoadFloat3(&nextFrame.vTranslation), 1.f), fRatio);
			return lerpedFrame;
		}

		_XMKeyFrame LerpKeyFram(const tagKeyFrame& nextFrame, _float nowTrackPosition, _float Distance) {
			_XMKeyFrame lerpedFrame = {};
			_float fRatio = nowTrackPosition / Distance;

			_float4 nextRot = nextFrame.vRotation;

			_vector nextRotation = XMLoadFloat4(&nextRot);
			_vector nowRotation = XMLoadFloat4(&vRotation);

			if (XMVector4Equal(nextRotation, XMVectorZero()))
				nextRotation = XMQuaternionIdentity();
			if (XMVector4Equal(nowRotation, XMVectorZero()))
				nowRotation = XMQuaternionIdentity();

			lerpedFrame.vScale = XMVectorLerp(XMLoadFloat3(&vScale), XMLoadFloat3(&nextFrame.vScale), fRatio);
			lerpedFrame.vRotation = XMQuaternionSlerp(nowRotation, nextRotation, (float)fRatio);
			lerpedFrame.vTranslation = XMVectorLerp(XMVectorSetW(XMLoadFloat3(&vTranslation), 1.f), XMVectorSetW(XMLoadFloat3(&nextFrame.vTranslation), 1.f), fRatio);
			return lerpedFrame;
		}

		tagKeyFrame& operator=(const tagKeyFrame& rhs)
		{
			if (this == &rhs) return *this;

			vScale = rhs.vScale;
			vRotation = rhs.vRotation;
			vTranslation = rhs.vTranslation;
			fTrackPosition = rhs.fTrackPosition;

			return *this;
		}
	}KEYFRAME;

	/*BoundingBox*/

	typedef struct tagMinMaxBoxInfo {
		_float3 vMin = {};
		_float3 vMax = {};

		tagMinMaxBoxInfo TransToWorld(_float4x4 worldMat) {
			tagMinMaxBoxInfo newBox = {};
			_matrix world = XMLoadFloat4x4(&worldMat);
			_vector min = XMLoadFloat3(&vMin);
			_vector max = XMLoadFloat3(&vMax);
			XMStoreFloat3(&newBox.vMin, XMVector3TransformCoord(min, world));
			XMStoreFloat3(&newBox.vMax, XMVector3TransformCoord(min, world));
			return newBox;
		}
	}MINMAX_BOX;

	/*RayInfo*/
	typedef struct tagRayInfo {
		_float3 vRayOrigin = {};
		_float3 vRayDirection = {};
		_float fMaxDistance = {};
	}RAY;

	/*RayHitInfo*/
	typedef struct tagRayHitInfo {
		class CGameObject* pObject = { nullptr };
		_float fDistance = {};
		_float3 vHittedPosition = {};
	}RAY_HIT;

	// Physics Ray Info
	typedef struct tagPhysicsRayInfo {
		_float3 vOrigin = {};                 // 시작점
		_float3 vDirection = {};              // 방향 (정규화됨)
		_float	fMaxDistance = 1000.f;        // 최대 거리
		_uint	iCollisionMask = 0xFFFFFFFF;  // 충돌 레이어 마스크
		_bool	bQueryTrigger = false;        // 트리거도 검사할지 여부
		_uint	iMaxHits = 1;                 // 최대 검출 개수
	}PHYSICS_RAY;

	// Physics Hit Info
	typedef struct tagPhysicsRayHitInfo {
		_bool				bHit = false;          // 충돌 여부
		_float				fDistance = 0.f;       // 충돌 지점까지의 거리
		_float3				vPoint = {};           // 충돌 지점 월드 좌표
		_float3				vNormal = {};          // 충돌 표면의 법선
		class CGameObject* pHitObject = nullptr;  // 충돌한 오브젝트
		class ICollidable* pCollidable = nullptr; // 충돌한 컴포넌트
		PxShape* pShape = nullptr;      // 충돌한 Shape
	}PHYSICS_RAY_HIT;

	// Physics Hits Info
	typedef struct tagPhysicsRayHitsInfo {
		_uint iHitCount = 0;
		vector<PHYSICS_RAY_HIT> vecHits;

		void Clear()
		{
			iHitCount = 0;
			vecHits.clear();
		}
	}PHYSICS_RAY_HITS;


	typedef struct tagInstanceInitDESC {
		_uint instanceStride = {};		// 인스턴스 구조체 크기 
		_uint instanceCount = {};     // 최대 인스턴스 개수
		_uint ElementCount = {};
		const D3D11_INPUT_ELEMENT_DESC* pElementDesc = { nullptr };
		string ElementKey = {  };
	}INSTANCE_INIT_DESC;

	struct COLLIDER_SLOT
	{
		enum class STATE : _uint
		{
			NONE = 0,   // 비어 있음 (슬롯 미사용)
			ACTIVE = 1,   // 충돌 검사 대상
			INACTIVE = 2,   // 일시 비활성 (충돌 검사 제외)
			DEAD = 3,   // 소유자가 삭제됨, 완전히 제거 예정
		};

		class CCollider* pCollider;
		STATE eState = STATE::NONE;
		_uint iGeneration = {};

		bool IsValid() const
		{
			if (eState == STATE::DEAD)
				return false;
			if (pCollider == nullptr)
				return false;
			return true;
		}

		bool IsActive() const { return eState == STATE::ACTIVE && pCollider != nullptr; }
	};

	typedef struct tagCollisionInfoContext {
		string EventTag = {};
		class CGameObject* Owner = { nullptr };
	}COLLISION_CONTEXT;

	typedef struct tagUITextInfo {
		string		TextKey;
		wstring		Text = {};
		_float2		TextPos = {};
		_float4		TextColor = {};
		string FontTag;
		_float Scale = 1.f;
		_float Rotation = 0.f;
		_float2 Origin = { 0.f, 0.f };

		_bool OutLined = { false };
		_float Thickness = 0.f;
		_float4 OutLineColor = {};

		_float2 vShear = {};
	}TEXT_INFO;


	typedef struct MaterialAnimationClip {
		_bool isLoop;
		_float fDuration;
		_float TickperSecond;
		vector<_uint> AnimationKeyFrame;
		vector<_float> FramePercent;
	}MATERIAL_CLIP;

	typedef struct MaterialAnimationKeyFrame {
		MATERIAL_CLIP Cilp;
		_float fCurrentTime = {};

		void Reset() { fCurrentTime = 0.f; }
	}MAT_KEYFRAME;

	struct RenderTargetDesc
	{
		string Key;

		DXGI_FORMAT ColorFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI_FORMAT DepthFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		_float4 vClearColor = { 0.f,0.f,0.f,0.f };
		_uint Width = 1280;
		_uint Height = 720;
	};

	/* Effect */
	typedef struct tagEffectNode : public INIT_DESC
	{
		_uint eType = static_cast<_uint>(EFFECT_TYPE::END);
		_float fDelayTime{};
		_float fDuration{};
		_bool isLoop = false;
	}EFFECT_NODE;

	typedef struct tagSpriteNode : public tagEffectNode
	{
		string TextureKey{};
		string TexturePath{};

		_bool isAnimated = false;
		_uint iMaxFrameIndex{};
		_float fSpeed{};
	}SPRITE_NODE;

	typedef struct tagParticleNode : public tagEffectNode
	{
		string TextureKey{};
		string TexturePath{};

		//_bool isLoop = false; 부모 구조체에서 루프 제어함
		_uint iModuleMask{};
		_uint iColorMode{};
		_bool isWorld = true;
		_uint iBurstCount{};
		_float fSpawnPerSec;
		_uint iMaxSpawnParticleCount{};

		_float2 vStartSpeed{};
		_float2 vStartLifeTime{};
		_float2 vStartSize{};

		_uint SpawnShape{};
		_float3 vCenter{};
		_float3 vHalfBox{};
		_float fRadius{};

		_bool useGravity = false;
		_float fGravityScale{};

		/*Life Time Velocity*/
		_float fDampScale{};

		/*Life Time Size*/
		_float2 vStartScale{};
		_float2 vEndScale{};

		/*Life Time Color*/
		_float4 vStartColor{};
		_float4 vEndColor{};

		/* Life Time Alpha */
		_float4 vAlphaKey{ 1.f,1.f,1.f,1.f };
		_float2 vRatio{ 0.3f,0.6f };

		/*Texture Sheet Animation*/
		_bool isParticleAnimated = false;
		_bool isRandomFrameIndex = false;
		_uint iCol{ 1 };
		_uint iRow{ 1 };
		_uint iMaxFrameIndex{};

		/*Noise*/
		_float3 vStrength{};
		_float3 vFrequency{};
		_float3 vScrollSpeed{};

		static tagParticleNode FromJson(nlohmann::ordered_json& json);
	}PARTICLE_NODE;

	typedef struct tagMeshNode : public tagEffectNode
	{
		string ModelTag{};
		string MaterialTag{};

		/* Texture Slot */
		_uint SamplerMode{};
		_uint MainUsage{};
		_uint Red{};
		_uint Green{};
		_uint Blue{};
		_uint Alpha{};

		/* Color */
		_uint ColorEaseType{};
		_float4 vStartColor{ 1.f,1.f,1.f,1.f };
		_float4 vEndColor{ 1.f,1.f,1.f,1.f };

		/* Scale */
		_uint ScaleEaseType{};
		_float3 vStartScale{ 1.f,1.f,1.f };
		_float3 vEndScale{ 1.f,1.f,1.f };

		/* UV Anim */
		_uint UVEaseType{};
		_float2 vStartUVOffset{};
		_float2 vEndUVOffset{};

		/* Sprite Animation */
		_uint iCol{ 1 };
		_uint iRow{ 1 };
		_uint iMaxFrameIndex{ 1 };

		/* Dissolve */
		_uint DissolveEase{};
		_float fDissolveStartProgress{};
		_float fDissolveEndProgress{};

		/* Bloom */
		_float fBloomIntensity{};

		static tagMeshNode FromJson(nlohmann::ordered_json& json);
	}MESH_NODE;

	typedef struct tagTrailNode : public tagEffectNode
	{
		_uint iMode{};
		_uint iTextureMode{};
		_uint iColorMode{};
		_float fMaxLifeTime{};

		/* Texture Mode */
		_float2 vUVSpeed{};
		_float fTile{};

		/* Color Mode */
		_float4 vStartColor{};
		_float4 vEndColor{};

		/* Center Mode */
		_float fStartWidth{};
		_float fEndWidth{};

		/* Segment Mode */
		_float fMinDistance{};

		static tagTrailNode FromJson(nlohmann::ordered_json& json);
	}TRAIL_NODE;

	typedef struct tagEffectAsset : public INIT_DESC
	{
		_uint iNodeCount{};
		_float fDuration{};
		_bool isLoop = false;
		vector<tagEffectNode*> Nodes;

		static tagEffectAsset FromJson(nlohmann::ordered_json& json);
	}EFFECT_ASSET;

	typedef struct ENGINE_DLL tagObjectHandle {
		string Level = {};
		string Layer = {};
		_uint hObjID = {};

		_bool isValid();
		void Reset();
		class CGameObject* Get();
		void Delete();
		_bool operator==(const tagObjectHandle& rhs) {
			return hObjID == rhs.hObjID;
		}
		tagObjectHandle& operator= (const tagObjectHandle& rhs) {
			Level = rhs.Level;
			Layer = rhs.Layer;
			hObjID = rhs.hObjID;
			return *this;
		}
		class CGameObject* operator()() { return Get(); }
		template<typename TObject>
		TObject* GetAs() const
		{
			static_assert(std::is_pointer_v<TObject*> == false, "TObject must be a type, not a pointer type.");
			CGameObject* objectPtr = Get();
			if (!objectPtr) return nullptr;
			return dynamic_cast<TObject*>(objectPtr);
		}

	}OBJECT_HANDLE;

	typedef struct ENGINE_DLL tagUIHandle {
		string Level = {};
		_uint hObjID = {};

		_bool isValid();
		void Reset();
		class CUI_Object* Get();
		void Release();
	}UI_HANDLE;

	struct IK_CONTEXT
	{
		class CAnimator3D*		pAnimator;
		vector<_int>			BoneIndices;
		_vector3				vPoleVector;
		_float					fWeight;
		vector<_quaternion>     OutRotations;
		vector<_vector3>		OutPositions;
		_bool					bSuccess;

		IK_CONTEXT()
			: pAnimator(nullptr)
			, vPoleVector(0.f, 0.f, 1.f)
			, fWeight(1.f)
			, bSuccess(false)
		{
		}
	};

	typedef struct tagLevelTransitionArgument {
		string nextLevelKey = {};
		_bool useLoading = { false };
		_bool KeepResource = { false };
		void Reset() { nextLevelKey.clear(); useLoading = false; KeepResource = false; }
	}LEVEL_TRANS_DESC;

}


#endif // Engine_Struct_h__