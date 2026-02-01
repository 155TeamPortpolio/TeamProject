#include "Animator3DEX.h"
#include "AnimationClip.h"
#include "GameInstance.h"
#include "Helper_Func.h"
#include "ModelData.h"

CAnimator3DEX::CAnimator3DEX()
{
}

CAnimator3DEX::CAnimator3DEX(const CAnimator3DEX& rhs)
	: CAnimator3D(rhs)
{
}

HRESULT CAnimator3DEX::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAnimator3DEX::Initialize(COMPONENT_DESC* pArg)
{
	return S_OK;
}

void CAnimator3DEX::Render_GUI()
{
	__super::Render_GUI();
}

void CAnimator3DEX::Update_Animation(_float dt)
{
	__super::Update_Animation(dt);

	//if (m_AnimLayers.empty()) return;
	//
	//for (auto& Layer : m_AnimLayers) {
	//	if (-1 == Layer.iClipIndex) continue;
	//
	//	auto& nowClip = m_pAnimClips[Layer.iClipIndex];

	//	nowClip->Check_Event(Layer.fCurrentTrackPosition, fTrackPosition, m_EventBus);

	//	Layer.fCurrentTrackPosition = fTrackPosition;
	//	nowClip->TranslateAnimateMatrixFromDuration(Layer.LocalMatrices, fTrackPosition, m_EventBus);

	//	//Eliminate Transform
	//	if (false == Layer.bUseTransform) {
	//		if (-1 != Layer.iMoveBoneIndex) {
	//			_float4x4& mat = Layer.LocalMatrices[Layer.iMoveBoneIndex];
	//			//지금은 Transform만 가져오고 있지만 혹시 회전이나 크기가 필요하면 매트릭스 자체를 저장해도 무관
	//			Layer.vPrevAnimPos = _float3(mat._41, mat._42, mat._43);
	//			mat._41 = mat._42 = mat._43 = 0;
	//		}
	//	}
	//}
	//
	//BuildBone();
}

HRESULT CAnimator3DEX::Link_MetaData(const string& LevelKey, const string& MetaClipKey)
{
	string metaPath = "../../Resources/Data/Meta/" + MetaClipKey;
	ANIM_META MetaData = Helper::LoadJson<ANIM_META>(metaPath);

	ANIMATION_META Meta;

	string AnimPath = MetaData.AnimPath;
	if (AnimPath.rfind("../", 0) == 0)
		AnimPath.erase(0, 3);

	const string animDir = "../../Client/" + AnimPath;
	Meta.PreTransform = MetaData.PreTransform;
	Meta.pClips.reserve(MetaData.Clips.size());


	for (auto& DataClip : MetaData.Clips)
	{
		string animPath = animDir + DataClip.ClipTag + ".anim";
		CAnimationClip* pClip = CAnimationClip::Create(animPath);

		if (!DataClip.Events.empty())
			pClip->Set_Events(DataClip.Events);

		if (pClip)
			Meta.pClips.push_back(pClip);
	}

	if (Meta.pClips.empty()) {
		string msg = "Anim Add Failed: " + MetaClipKey + "\n";
		OutputDebugStringA(msg.c_str());
		return E_FAIL;
	}

	m_AnimPath = MetaData.AnimPath;
	m_PreTransform = Meta.PreTransform;
	m_pAnimClips = Meta.pClips;


	Resize_Layer(1);
	//0번 레이어는 베이스레이어, Layer.BaseLayer는 웬만하면 건들지 말것
	m_AnimLayers[0].BaseLayer = true;
	m_AnimLayers[0].eLayerType = ANIM_LAYER_STATE::BASE;
	m_AnimLayers[0].fLayerWeight = 1.f;
	m_AnimLayers[0].iRootBoneIndex = m_pData->Find_BoneIndexByName("Root");

	BuildBone();
	m_TPose = m_CombinedMatrices;

	return S_OK;
}

void CAnimator3DEX::Create_Clips()
{
}

vector<class CAnimationClip*>* CAnimator3DEX::Get_Clips()
{
	return &m_pAnimClips;
}

vector<ANIM_LAYER>& CAnimator3DEX::Get_AnimLayers()
{
	return m_AnimLayers;
}

CAnimator3DEX* CAnimator3DEX::Create()
{
	CAnimator3DEX* instance = new CAnimator3DEX();
	if (FAILED(instance->Initialize_Prototype())) {
		Safe_Release(instance);
	}
	return instance;
}

CComponent* CAnimator3DEX::Clone()
{
	return new CAnimator3DEX(*this);
}

void CAnimator3DEX::Free()
{
	__super::Free();
}
