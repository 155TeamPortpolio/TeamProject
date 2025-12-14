#pragma once
namespace Engine {
	/*Base Clone Desc*/
	typedef struct tagGameObjectCloneDesc : public INIT_DESC {
		string OriginLevel;
		string protoTag;

		tagGameObjectCloneDesc(const string& _originLevel, const string& _protoTag) :protoTag(_protoTag), OriginLevel(_originLevel) {}
		tagGameObjectCloneDesc() {};
		virtual ~tagGameObjectCloneDesc() DEFAULT;
	}CLONE_DESC;

	typedef struct LayerAddDesc : public INIT_DESC
	{
		string LevelTag;
		string LayerTag;
		LayerAddDesc(const string& _destLevel, const string& _layerTag) :LayerTag(_layerTag), LevelTag(_destLevel) {}
		virtual ~LayerAddDesc() DEFAULT;
	}LAYER_DESC;

	/*Component Clone Desc*/
	typedef struct tagComponentDesc : public INIT_DESC {
		tagComponentDesc() {};
		virtual ~tagComponentDesc() DEFAULT;
	}COMPONENT_DESC;

	typedef struct TransformInitDesc :public COMPONENT_DESC {
		_float3 vInitialPosition = {};
		_float3 vInitialEulerVector = {};
		_float3 vInitialScale = { 1.f,1.f, 1.f };

		TransformInitDesc() DEFAULT;
		virtual ~TransformInitDesc() DEFAULT;
	}TRANSFORM_DESC;

	typedef struct RigidBodyinitDesc : public COMPONENT_DESC {
		_bool		isStatic = false;		// 움직이는 물체 / 안움직이는 물체
		_bool       isKinematic = false;    // Transform을 통한 제어, 물리 엔진 제어
		_bool       bEnableGravity = true;	// 중력 적용
		_float		fMass = 1.0f;			// 질량
		_bool		bLockX = true;
		_bool		bLockY = false;
		_bool		bLockZ = true;
		string		strMaterialTag = {};

		RigidBodyinitDesc() DEFAULT;
		virtual ~RigidBodyinitDesc() DEFAULT;
	}RIGIDBODY_DESC;

	typedef struct CameraInitDesc :public COMPONENT_DESC {
		_float fNear = {1.f};
		_float fFar = {500};
		_float fAspect = {};
		_float fFov = {60.f};
		CameraInitDesc() DEFAULT;
		CameraInitDesc(_float fFar, _float fNear, _float fAspect) : fNear{ fNear }, fFar{ fFar }, fAspect{ fAspect } {};
		CameraInitDesc(_float fAspect) : fAspect{ fAspect } {};
		virtual ~CameraInitDesc() DEFAULT;
	}CAMERA_DESC;

	typedef struct LightInitDesc :public COMPONENT_DESC {

		LIGHT_TYPE			eType = {};
		_float4		vDiffuse = {};
		_float4		vAmbient = {};
		_float4		vSpecular = {};

		_float4		vDirection = {};
		_float4		vPosition = {};
		_float			fRange = {};


		LightInitDesc() DEFAULT;
		virtual ~LightInitDesc() DEFAULT;

	}LIGHT_INIT_DESC;

	typedef struct ColliderInitDesc :public COMPONENT_DESC {
		COLLIDER_TYPE eType = COLLIDER_TYPE::BOX;
		_float3 vCenter = { 0.f, 0.f, 0.f };	// 로컬 오프셋
		_float3 vSize = { 1.f, 1.f, 1.f };		// Box: HalfExtents(x,y,z), Sphere: Radius(x), Capsule: Radius(x)/HalfHeight(y)
		_float3 vRotation = { 0.f, 0.f, 0.f };	// 로컬 회전 (Radian)
		_bool   isTrigger = false;				// 트리거 여부
		string  strMaterialTag = "";			// 재질 태그
	}COLLIDER_DESC;

	//typedef struct ColliderAABBInitDesc :public COLLIDER_DESC {
	//	_float3 vSize = {};
	//	ColliderAABBInitDesc() DEFAULT;
	//	ColliderAABBInitDesc(_float3 vCenter, _float3 vSize) :COLLIDER_DESC{ vCenter }, vSize{vSize} {};
	//	ColliderAABBInitDesc(const ColliderAABBInitDesc& rhs) :COLLIDER_DESC{ rhs }, vSize{ rhs.vSize} {};
	//	virtual ~ColliderAABBInitDesc() DEFAULT;
	//}AABB_COLLIDER_DESC;

	//typedef struct ColliderInitOBBDesc :public COLLIDER_DESC {
	//	_float3 vSize = {};
	//	_float3 vEularRadians = {};
	//	ColliderInitOBBDesc() DEFAULT;
	//	virtual ~ColliderInitOBBDesc() DEFAULT;
	//	ColliderInitOBBDesc(_float3 vCenter, _float3 vSize, _float3 vEularRadians) :COLLIDER_DESC{ vCenter }, vSize{ vSize }, vEularRadians{ vEularRadians } {};
	//	ColliderInitOBBDesc(const ColliderInitOBBDesc& rhs) :COLLIDER_DESC{ rhs }, vSize{ rhs.vSize } , vEularRadians{rhs.vEularRadians } {};
	//}OBB_COLLIDER_DESC;

	//typedef struct ColliderInitSphereDesc :public COLLIDER_DESC {
	//	_float fRadius = {};
	//	ColliderInitSphereDesc() DEFAULT;
	//	virtual ~ColliderInitSphereDesc() DEFAULT;
	//	ColliderInitSphereDesc(_float3 vCenter, _float vRadius) :COLLIDER_DESC{ vCenter }, fRadius{ vRadius }{};
	//	ColliderInitSphereDesc(const ColliderInitSphereDesc& rhs) :COLLIDER_DESC{ rhs }, fRadius{ rhs.fRadius }{};
	//}SPHERE_COLLIDER_DESC;

	/*Parent Child Desc*/
	typedef struct tagSetParentDesc :public COMPONENT_DESC {
		class CGameObject* pParent = { nullptr };
		tagSetParentDesc(CGameObject* pParent) : pParent{pParent} {};
		virtual ~tagSetParentDesc() DEFAULT;
	}PARENT_DESC;

	/*Object Clone Desc*/
	typedef struct tagGameObjectDesc : public INIT_DESC {
		string InstanceName = "";
		unordered_map<type_index, COMPONENT_DESC*> CompDesc;

		virtual ~tagGameObjectDesc() DEFAULT;
	}GAMEOBJECT_DESC;

	typedef struct tagUIObjectDesc : public GAMEOBJECT_DESC {
		_float fX = {};
		_float fY = {  };
		_float fSizeX = {1.f };
		_float fSizeY = { 1.f };
		_float fRadian = {};
		tagUIObjectDesc() DEFAULT;
		tagUIObjectDesc(const _float4& ui_Info) :fX(ui_Info.x), fY(ui_Info.y), fSizeX(ui_Info.z), fSizeY(ui_Info.w), GAMEOBJECT_DESC{} {}
		virtual ~tagUIObjectDesc() DEFAULT;
	}UI_DESC;

}