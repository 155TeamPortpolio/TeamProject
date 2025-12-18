#pragma once
#include "Engine_Defines.h"

namespace Engine {

	typedef struct tagRenderKey {
		_uint ShaderID = {};
		_uint MaterialID = {};

		bool operator<(const tagRenderKey& rhs) {
			if (ShaderID != rhs.ShaderID) return ShaderID < rhs.ShaderID;
			if (MaterialID != rhs.MaterialID) return MaterialID < rhs.MaterialID;
			return false;
		}
	}RENDERKEY;

	typedef struct tagShadowRenderKey {
		_uint ShaderID = {};

		bool operator<(const tagShadowRenderKey& rhs) {
			if (ShaderID != rhs.ShaderID) return ShaderID < rhs.ShaderID;
			return false;
		}
	}SHADOWKEY;

	typedef struct DrawBasePacket {
		_float4x4* pWorldMatrix;
		_uint TransformIndex = {};
	}BASE_PACKET;

	typedef struct DrawPriorityPacket {
		
	}PRIORITY_PACKET;

	/*������ ��Ŷ*/
	typedef struct DrawOpaquePacket : BASE_PACKET {
		_bool bSkinning = { false }; /*�׷��� ���� �ִ�?*/

		_uint DrawIndex = {};		/*���° �޽� �׸��µ�?*/
		_uint MaterialIndex = {};/*�� �޽ô� �����µ�*/
		_uint SkinningOffset = {};

		class CModel* pModel = { nullptr };
		class CMaterial* pMaterial = { nullptr };

		variant<monostate, class CAnimator3D*, class CSkeletonFollower*> pPayLoad; /*�߰������� �ְ� ���� �� �־�?*/

		RENDERKEY GetKey() const;
	}OPAQUE_PACKET;

	/*�ν��Ͻ� ��Ŷ*/
	typedef struct DrawInstancePacket : BASE_PACKET {
		_uint DrawIndex = {};			/*���° �޽� �׸��µ�?*/
		_uint MaterialIndex = {};		/*�� �޽ô� �����µ�*/		
		_float4x4* pWorldMatrix;
		class CInstanceModel* pModel = { nullptr };
		class CMaterial* pMaterial = { nullptr };
	}INSTANCE_PACKET;
	

	typedef struct tagBlendedRenderKey {
		_uint ShaderID = {};
		_uint MaterialID = {};
		_float DistanceToCamera = {};

		bool operator<(const tagBlendedRenderKey& rhs) {
			if (DistanceToCamera != rhs.DistanceToCamera)
				return DistanceToCamera > rhs.DistanceToCamera;

			if (ShaderID != rhs.ShaderID) return ShaderID < rhs.ShaderID;
			if (MaterialID != rhs.MaterialID) return MaterialID < rhs.MaterialID;
			return false;
		}
	}BLENDRENDERKEY;

	/*������ ��Ŷ*/
	typedef struct DrawBlendedPacket : BASE_PACKET {
		_bool bSkinning = { false }; /*�׷��� ���� �ִ�?*/

		_uint DrawIndex = {};		/*���° �޽� �׸��µ�?*/
		_uint MaterialIndex = {};/*�� �޽ô� �����µ�*/
		_uint SkinningOffset = {};

		class CModel* pModel = { nullptr };
		class CMaterial* pMaterial = { nullptr };

		variant<monostate, class CAnimator3D*, class CSkeletonFollower*> pPayLoad; /*�߰������� �ְ� ���� �� �־�?*/

		_float DistanceToCamera = 0.f;       // ���Ŀ� Key
		BLENDRENDERKEY GetKey() const;
	}BLENDED_PACKET;

	/*Particle*/
	typedef struct DrawParticlePacket : BASE_PACKET {
		_float4x4 WorldMatrix{};
		class CParticleSystem* pParticleSystem = { nullptr };
		class CMaterial* pMaterial = { nullptr };
	}PARTICLE_PACKET;

	typedef struct DrawUIPacket : BASE_PACKET {
		class CSprite2D* pSprite2D = { nullptr };
	}SPRITE_PACKET;

	typedef struct DrawDebugPacket : BASE_PACKET {
		class CModel* pModel = { nullptr };
		class CDebugRender* pDebug = { nullptr };
		_uint DrawIndex = {}; /*���° �޽� �׸��µ�?*/
	}DEBUG_PACKET;

	/*Audio*/
	typedef struct tagAudioPacket {
		_bool isInfinite = { false }; 
		_bool isPaused = { false };
		_bool is3DAttribute = { true };
		_uint iLoopCount = { 0};

		_float fVolume = { 1.f };
		SOUND_GROUP eGroup = {};

		class CSoundData* pSound = {};
		FMOD::Channel** ppChannelToUpdate = { nullptr };
		_float3 vPosition = {};
	}AUDIO_PACKET;

	/*Shader Param*/
	typedef struct tagShaderParameter {
		void* pData = { nullptr };
		string typeName = {};
		_uint iSize = {};
	}SHADER_PARAM;


	typedef struct tagCustomRenderRequestCommand {
		string TargetKey;                    
		function<void(ID3D11DeviceContext*)> DrawCallback;  
	}RENDER_CUSTOM_COMMAND;

	typedef struct RenderPostProcessingRequestCommand
	{
		//string MrtKey;
		vector<POSTPROCESS> Targets; //	enum class POSTPROCESS { BLOOM, DISTORTION, END};
		class CShader* pShader = { nullptr };
		_float4x4* pWorldMatrix = { nullptr };
		function<void(ID3D11DeviceContext*)> DrawCall;
		_bool bClearColor = false;
		_bool bClearDepth = false;
	}POST_PROCESS_COMMAND;
}
