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
		_float4		vLightPosition = {};
		_float4		vLightDirection = {};
		_float4		vLightDiffuse = {};
		_float4		vLightAmbient = {};
		_float4		vLightSpecular = {};
		_float			fLightRange = {};
		_float3		lightPadding = {};
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
	}MESH_INFO_HEADER;

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
	typedef struct ENGINE_DLL tagAnimationClipHeader {
		_bool					bLoop = { };
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
		_float				fTrackPosition;

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
			XMStoreFloat3(&newBox.vMin, XMVector3TransformCoord(min,world));
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
		wstring		Text = {};
		_float2		TextPos = {};
		_float4		TextColor = {};
		string FontTag;
		_float Scale = 1.f;
		_float Rotation = 0.f;
		_float2 Origin = { 0.f, 0.f }; // 회전읮 중점

		_bool OutLined = { false };
		_float Thickness = 0.f;
		_float4 OutLineColor = {};
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

	typedef struct tagEffectNode
	{
		_float fDelayTime{};
		_float fDuration{};
		_bool isLoop = false;
	}EFFECT_NODE;

	typedef struct tagSpriteNode : public tagEffectNode
	{

	}SPRITE_NODE;

	typedef struct tagParticleNode : public tagEffectNode
	{

	}PARTICLE_NODE;

	typedef struct tagEffectAsset
	{
		string Name{};
		_float fDuration{};
		vector<tagEffectNode> Nodes;
	}EFFECT_ASSET;
}


#endif // Engine_Struct_h__