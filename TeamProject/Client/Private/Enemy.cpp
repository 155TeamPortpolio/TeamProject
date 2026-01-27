#include "pch.h"
#include "Enemy.h"
#include "GameInstance.h"
#include "BattleSystem.h"

/* Object */
#include "AttackSign.h" 
#include "EnemyAttackCollider.h"
#include "UI_EnemyStatus.h"
#include "UI_BossHUD.h"
#include "UI_DamageText.h"
#include "UIDirector.h"

/* Component */
#include "ObjectContainer.h"
#include "Animator3D.h"
#include "BoneFollower.h"
#include "Material.h"
#include "MaterialInstance.h"

CEnemy::CEnemy()
	:CGameObject()
{
}

CEnemy::CEnemy(const CEnemy& rhg)
	:CGameObject(rhg)
{
}

HRESULT CEnemy::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	Add_Component<CObjectContainer>();

	return S_OK;
}

HRESULT CEnemy::Initialize(INIT_DESC* pArg)
{
	__super::Initialize(pArg);

	ENEMY_DESC* pDesc = static_cast<ENEMY_DESC*>(pArg);

	m_tStatus.iMaxHP = m_tStatus.iNowHP = pDesc->iMaxHP;

	switch (m_eEnemyClass)
	{
	case Client::CEnemy::ENEMY_CLASS::NORMAL:
		m_tStatus.iPlayerComboCount = 1;
		break;
	case Client::CEnemy::ENEMY_CLASS::ELITE:
		m_tStatus.iPlayerComboCount = 2;
		break;
	case Client::CEnemy::ENEMY_CLASS::BOSS:
		m_tStatus.iPlayerComboCount = 3;
		break;
	}

	return S_OK;
}

void CEnemy::Awake()
{
	m_fDissolveProgress = 0.f;
	m_fDissolveTilling = 1.f;

	auto pMaterial = Get_Component<CMaterial>();
	auto& materialInstances = pMaterial->Get_MaterialInstances();
	auto dissolveTexture = ResourceManager()->Load_Texture(G_GlobalLevelKey, "Dissolve.png");

	for (const auto& instance : materialInstances)
	{
		instance->Set_Param("fDissolveProgress", {&m_fDissolveProgress, "float", sizeof(_float)});
		instance->Set_Param("fDissolveTiling", {&m_fDissolveTilling, "float", sizeof(_float)});
	}
}

void CEnemy::Update(_float dt)
{
	Get_Component<CObjectContainer>()->UpdateChild(dt);
	CheckAutoBattlePlay(dt);
	ManageGroggy(dt);

	m_PlayerCharacterInfos.clear();
	m_PlayerCharacterInfos = CBattleSystem::GetInstance()->GetBattleObjects(CBattleSystem::BATTLE_OBJ_TYPE::PLAYER);
	ComputeTargetingInfo();
}

void CEnemy::Late_Update(_float dt)
{
	Get_Component<CObjectContainer>()->Late_UpdateChild(dt);
}

BATTLEOBJ_INFO* CEnemy::GetCharacterOnField()
{
	for (auto& info : m_PlayerCharacterInfos) {
		if (true == info.isOnField &&
			info.hObject == CBattleSystem::GetInstance()->GetCurCharacterHandle())
			return &info;
	}
	return nullptr;
}

void CEnemy::ComputeTargetingInfo()
{
	auto pTargetInfo = GetCharacterOnField();
	if (nullptr == pTargetInfo)
		return;

	m_tTargetingInfo = {};

	m_tTargetingInfo.vTargetPos = pTargetInfo->vPos;
	m_tTargetingInfo.vSelfPos = m_pTransform->Get_Pos();
	m_tTargetingInfo.vDirSelfLook = m_pTransform->Dir(Engine::STATE::LOOK);
	m_tTargetingInfo.vDirSelfLook.Normalize();

	// Y축 제거 한 수평 방향 벡터 계산 버전(XZ평면)
	_vector3 vTargetPosH = { m_tTargetingInfo.vTargetPos.x, 0.f, m_tTargetingInfo.vTargetPos.z };
	_vector3 vSelfPosH = { m_tTargetingInfo.vSelfPos.x, 0.f, m_tTargetingInfo.vSelfPos.z };
	_vector3 vDirToTarget = vTargetPosH - vSelfPosH;

	// Y축 제거 안한 3D 방향 벡터 계산 버전(XYZ)
	//_vector3 vDirToTarget = m_tTargetingInfo.vTargetPos - m_tTargetingInfo.vSelfPos;

	m_tTargetingInfo.fDistanceSq = vDirToTarget.LengthSquared();
	//if (m_tTargetingInfo.fDistanceSq <= m_fDetectedRange * m_fDetectedRange)
	//	m_tTargetingInfo.isDetected = true;

	// sqrt 계산이 비교적 무거워서 후에 최적화 필요시 아래 식 사용 고려
	//m_tTargetingInfo.fDistance = (m_tTargetingInfo.fDistanceSq > m_fDetectedRange * m_fDetectedRange) ? 
	//	sqrt(m_tTargetingInfo.fDistanceSq) : m_fDetectedRange;
	m_tTargetingInfo.fDistance = sqrt(m_tTargetingInfo.fDistanceSq);

	// 혹시 모를 0 나누기 방지
	if (m_tTargetingInfo.fDistance > 1e-12f) {
		vDirToTarget.Normalize();
	}
	m_tTargetingInfo.vDirToTarget = vDirToTarget;

	// 평면 XZ상 내적 계산
	_vector3 vSelfLookH = m_tTargetingInfo.vDirSelfLook;
	vSelfLookH.y = 0.f;
	vSelfLookH.Normalize();
	m_tTargetingInfo.fDotTarget = vSelfLookH.Dot(m_tTargetingInfo.vDirToTarget);

	// 3D공간상 내적(XYZ)
	//m_tTargetingInfo.fDotTarget = m_tTargetingInfo.vDirSelfLook.Dot(m_tTargetingInfo.vDirToTarget);
}

void CEnemy::Render_GUI_ForTargetInfo()
{
	ImGui::PushID(this);

	float childWidth = ImGui::GetContentRegionAvail().x;
	const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
	const float childHeight = (textLineHeight * 8) + (ImGui::GetStyle().WindowPadding.y * 2);

	ImGui::SeparatorText("For Target Information");
	auto pCharacter = GetCharacterOnField();
	if (nullptr != pCharacter) {
		ImGui::BeginChild("TracePlayer##ThugBulkyEnforcerTracePlayer", ImVec2{ 0, childHeight }, true);

		ImGui::Text("Character Name : %s", pCharacter->TagInstanceName.c_str());
		ImGui::Text("Character Pos : %.2f, %.2f, %.2f", m_tTargetingInfo.vTargetPos.x, m_tTargetingInfo.vTargetPos.y, m_tTargetingInfo.vTargetPos.z);
		ImGui::Text("Character CCT Radius : %.2f", pCharacter->fRadius);;
		ImGui::Text("Distance From Character : %.3f", m_tTargetingInfo.fDistance);
		ImGui::Text("Dot with Target : %.2f", m_tTargetingInfo.fDotTarget);
		ImGui::Text("Dir To Target : %.2f, %.2f, %.2f", m_tTargetingInfo.vDirToTarget.x, m_tTargetingInfo.vDirToTarget.y, m_tTargetingInfo.vDirToTarget.z);
		
		const _char* map[8] = { "F", "FR", "R", "BR", "B", "BL", "L", "FL" };
		ImGui::Text("DIR : %s", map[ENUM(GetDIRToPlayer())]);
		
		ImGui::BeginDisabled(true);
		ImGui::Checkbox(u8"isDetected", &m_tTargetingInfo.isDetected);
		ImGui::EndDisabled();

		ImGui::EndChild();
	}
	ImGui::PopID();
}

void CEnemy::Create_AttackSign(string boneTag)
{
	auto pObjectContainer = Get_Component<CObjectContainer>();
	auto pAnimator = Get_Component<CAnimator3D>();

	auto pAttackSign = Builder::Create_Object({ G_GlobalLevelKey,"Proto_GameObject_AttackSign" })
		.Build("AttackSign");
	pObjectContainer->Add_Child(pAttackSign, false);
	pAttackSign->Get_Component<CBoneFollower>()->Link_Bone(pAnimator, boneTag);
}

void CEnemy::Active_AttackSign(_bool parryEnable)
{
	auto pAttackSign = Get_Component<CObjectContainer>()->Find_ObjectByName("AttackSign");

	_bool IsReallyParryEnable = parryEnable;

	if (true == parryEnable) {
		if (BattleSystem()->GetPlayerParryingCount() <= 0)
			IsReallyParryEnable = false;
	}

	static_cast<CAttackSign*>(pAttackSign)->Active(IsReallyParryEnable);
}

void CEnemy::TakeDamage(DAMAGE_TYPE eDamageType, _float fDamage)
{
	_float fTakeDamage = fDamage;
	
	if (m_tStatus.isGroggy)
		fTakeDamage *= 1.5f;
	else
		m_tStatus.iGroggyValue += 2;

	m_tStatus.iNowHP -= fTakeDamage;

	if (0 >= m_tStatus.iNowHP)
		m_tStatus.iNowHP = 0.f;

	CUI_DamageText::DAMAGE_DESC desc{};
	desc.damage        = (_int)fTakeDamage;
	desc.followHandle  = Get_Handle();
	desc.followOffset  = Vector3(0.f, 1.3f, 0.f);
	desc.isEnemy       = true;

	UIDirector()->Request_DamageText(&desc);
}

void CEnemy::Create_UIEnemyStatus(string boneTag)
{
	// 월드 행렬 포인터 
	if (!m_pTransform)
		return;

	const _float4x4* pParentWorld = m_pTransform->Get_WorldMatrix_Ptr();
	if (!pParentWorld)
		return;

	// 본 로컬 행렬 포인터
	const _float4x4* pBoneLocal = Get_Component<CAnimator3D>()->Get_BoneMatrixPtr(CAnimator3D::BoneSpace::COMBINED, boneTag);
	if (!pBoneLocal)
		return;

	// ENEMYSTATUS_DESC 생성
	CUI_EnemyStatus::ENEMYSTATUS_DESC* pDesc = new CUI_EnemyStatus::ENEMYSTATUS_DESC;
	pDesc->pParentWorld = pParentWorld;
	pDesc->pBoneLocal = pBoneLocal;
	pDesc->pMonsterStatus = &m_tStatus;
	pDesc->tOwnerHandle = Get_Handle();

	// EnemyStatus UI 생성
	const string& strLevelKey = LevelManager()->Get_NowLevelKey();
	auto pEnemyStatus = Builder::Create_UIObject({ G_GlobalLevelKey,"Proto_GameObject_EnemyStatus" })
		.Add_UIDesc(pDesc)
		.Build("EnemyStatus");

	// UI Mgr에 등록
	CGameInstance::GetInstance()->Get_UIMgr()->Add_UIObject(pEnemyStatus, strLevelKey);

	m_hUIEnemyStatus = pEnemyStatus->Get_Handle();
}

void CEnemy::Create_UIBossHUD()
{
	// BOSS_HUD_DESC 생성
	CUI_BossHUD::BOSS_HUD_DESC* pDesc = new CUI_BossHUD::BOSS_HUD_DESC;
	pDesc->pMonsterStatus = &m_tStatus;

	// BossHUD UI 생성
	const string& strLevelKey = LevelManager()->Get_NowLevelKey();
	auto pBossHUD = Builder::Create_UIObject({ strLevelKey,"Proto_GameObject_BossHUD" })
		.Add_UIDesc(pDesc)
		.Build("bossHUD");

	// UI Mgr에 등록
	CGameInstance::GetInstance()->Get_UIMgr()->Add_UIObject(pBossHUD, strLevelKey);
}
HRESULT CEnemy::AttachBattleColliderObject(BATTLE_COLLIDER_DESC* pDesc, _bool isSeparate)
{
	//_bool           isAttachBone = { true };                // 뼈에 붙이는지
	//string          tagBone = "";                           // 뼈에 붙일때, 붙일 뼈의 이름
	//CAnimator3D* pOwnerAnimator3D = { nullptr };           // 뼈에 붙일때, Owner의 애니메이터 포인터
	//COLLIDER_TYPE   eAttackColliderType = COLLIDER_TYPE::SPHERE;  // BOX, SPHERE, CAPSULE
	///* 실제 Hit처리될 Attack용 콜라이더의 사이즈
	//Box: HalfExtents(x, y, z),
	//Sphere : Radius(x),
	//Capsule : Radius(x) / HalfHeight(y)*/
	//_float3         vAttackSize = { 1.f, 1.f, 1.f };
	///* Parrying 및 회피용 콜라이더의 사이즈. Attack용보다 사이즈 크게 할 것
	//Box: HalfExtents(x, y, z),
	//Sphere : Radius(x),
	//Capsule : Radius(x) / HalfHeight(y)*/
	//_float3         vTriggerSize = { 1.f, 1.f, 1.f };

	if (nullptr == pDesc)
		return E_FAIL;

	// 본에 붙일건데 애니메이터 안가져오면 BOOM! 다시가져오렴
	if (true == pDesc->isAttachBone && nullptr == pDesc->pOwnerAnimator3D)
		return E_FAIL;

	string tagNowLevel = CGameInstance::GetInstance()->Get_LevelMgr()->Get_NowLevelKey();

	auto pObjectContainer = Get_Component<CObjectContainer>();
	if (nullptr == pObjectContainer)
		return E_FAIL;

	RIGIDBODY_DESC rigidbodyDesc = {};
	rigidbodyDesc.bEnableGravity = false;
	rigidbodyDesc.isKinematic = true;
	rigidbodyDesc.bLockY = true;

//트리거 이제안씀
#pragma region TriggerCollider(X)
	//if (false == isSeparate)
	//{
	//	auto iter = m_BattleColliderChildrenIndex.find("DefaultTriggerCollider");
	//
	//	// DefaultTriggerCollider가 없을 때 생성
	//	if (iter == m_BattleColliderChildrenIndex.end())
	//	{
	//		COLLIDER_DESC TriggercolliderDesc = {};
	//		TriggercolliderDesc.eGroup = COLLISION_GROUP::MONSTER_PARRY;
	//		TriggercolliderDesc.iCollisionMask = ENUM(COLLISION_GROUP::PLAYER) | ENUM(COLLISION_GROUP::PLAYER_ATTACK);
	//		TriggercolliderDesc.bTrigger = true;
	//		TriggercolliderDesc.bAutoFit = false;
	//		TriggercolliderDesc.eType = pDesc->eTriggerColliderType;
	//		TriggercolliderDesc.vSize = pDesc->vTriggerSize;
	//		TriggercolliderDesc.fSizeScale = pDesc->fSizeScale;
	//		TriggercolliderDesc.vCenter = {}; //pDesc->vCenter;
	//		TriggercolliderDesc.vRotation = pDesc->vRotation;
	//
	//		string tagTriggerInstance = "DefaultTriggerCollider";
	//
	//		auto pTriggerCollider = Builder::Create_Object({ tagNowLevel, "Proto_GameObject_EnemyTriggerCollider" })
	//			.RigidBody(rigidbodyDesc)
	//			.Collider(TriggercolliderDesc)
	//			.Build(tagTriggerInstance);
	//
	//		if (nullptr == pTriggerCollider)
	//			return E_FAIL;
	//
	//		_int iTriggerColliderChildIndex = { -1 };
	//		// 뼈에 붙일 때
	//		//if (true == pDesc->isAttachBone) {
	//		iTriggerColliderChildIndex = pObjectContainer->Add_Child(pTriggerCollider, false);
	//		pTriggerCollider->Get_Component<CBoneFollower>()->Link_Bone(pDesc->pOwnerAnimator3D, "RootNode");
	//		//}
	//		//else
	//		//	iTriggerColliderChildIndex = pObjectContainer->Add_Child(pTriggerCollider, true);
	//
	//		m_BattleColliderChildrenIndex.emplace(tagTriggerInstance, iTriggerColliderChildIndex);
	//	}
	//}
	//else
	//{
	//	COLLIDER_DESC TriggercolliderDesc = {};
	//	TriggercolliderDesc.eGroup = COLLISION_GROUP::MONSTER_PARRY;
	//	TriggercolliderDesc.iCollisionMask = ENUM(COLLISION_GROUP::PLAYER) | ENUM(COLLISION_GROUP::PLAYER_ATTACK);
	//	TriggercolliderDesc.bTrigger = true;
	//	TriggercolliderDesc.bAutoFit = false;
	//	TriggercolliderDesc.eType = pDesc->eTriggerColliderType;
	//	TriggercolliderDesc.vSize = pDesc->vTriggerSize;
	//	TriggercolliderDesc.fSizeScale = pDesc->fSizeScale;
	//	TriggercolliderDesc.vCenter = pDesc->vCenter;
	//	TriggercolliderDesc.vRotation = pDesc->vRotation;
	//
	//	string tagTriggerInstance = pDesc->tagName + "_TriggerCollider";
	//
	//	auto pTriggerCollider = Builder::Create_Object({ tagNowLevel, "Proto_GameObject_EnemyTriggerCollider" })
	//		.RigidBody(rigidbodyDesc)
	//		.Collider(TriggercolliderDesc)
	//		.Build(tagTriggerInstance);
	//
	//	if (nullptr == pTriggerCollider)
	//		return E_FAIL;
	//
	//	_int iTriggerColliderChildIndex = { -1 };
	//	// 뼈에 붙일 때
	//	if (true == pDesc->isAttachBone) {
	//		iTriggerColliderChildIndex = pObjectContainer->Add_Child(pTriggerCollider, false);
	//		pTriggerCollider->Get_Component<CBoneFollower>()->Link_Bone(pDesc->pOwnerAnimator3D, pDesc->tagBone);
	//	}
	//	else
	//		iTriggerColliderChildIndex = pObjectContainer->Add_Child(pTriggerCollider, true);
	//
	//	m_BattleColliderChildrenIndex.emplace(tagTriggerInstance, iTriggerColliderChildIndex);
	//}

#pragma endregion


#pragma region AttackCollider
	COLLIDER_DESC AttackcolliderDesc = {};
	AttackcolliderDesc.eGroup = COLLISION_GROUP::MONSTER_ATTACK;
	AttackcolliderDesc.iCollisionMask = ENUM(COLLISION_GROUP::PLAYER) | ENUM(COLLISION_GROUP::PLAYER_ATTACK);
	AttackcolliderDesc.bTrigger = true;
	AttackcolliderDesc.bAutoFit = false;
	AttackcolliderDesc.eType = pDesc->eAttackColliderType;
	AttackcolliderDesc.vSize = pDesc->vAttackSize;
	AttackcolliderDesc.fSizeScale = pDesc->fSizeScale;
	AttackcolliderDesc.vCenter = pDesc->vCenter;
	AttackcolliderDesc.vRotation = pDesc->vRotation;

	string tagAttackInstance = pDesc->tagName + "_AttackCollider";

	auto pAttackCollider = Builder::Create_Object({ tagNowLevel, "Proto_GameObject_EnemyAttackCollider" })
		.RigidBody(rigidbodyDesc)
		.Collider(AttackcolliderDesc)
		.Build(tagAttackInstance);

	if (nullptr == pAttackCollider)
		return E_FAIL;

	_int iAttackColliderChildIndex = { -1 };
	// 뼈에 붙일 때
	if (true == pDesc->isAttachBone) {
		iAttackColliderChildIndex = pObjectContainer->Add_Child(pAttackCollider, false);
		pAttackCollider->Get_Component<CBoneFollower>()->Link_Bone(pDesc->pOwnerAnimator3D, pDesc->tagBone);
	}
	else
		iAttackColliderChildIndex = pObjectContainer->Add_Child(pAttackCollider, true);

	m_BattleColliderChildrenIndex.emplace(tagAttackInstance, iAttackColliderChildIndex);
#pragma endregion	


	return S_OK;
}
void CEnemy::ManageGroggy(const _float dt)
{
	if (false == m_tStatus.isGroggy && 100 <= m_tStatus.iGroggyValue)
	{
		m_tStatus.iGroggyValue = 100;
		m_tStatus.isGroggy = true;
		// UI 효과용
		m_tStatus.isGroggyStay = true;
	}

	if (true == m_tStatus.isGroggy)
	{
		if (true == m_tStatus.isGroggyStay)
		{
			m_tGroggyManage.fGroggyStayTime += dt;

			m_tStatus.iGroggyValue = Helper::Get_Random_Int(1, 99);
			if (m_tGroggyManage.fGroggyStayTime >= 3.f)
			{
				m_tStatus.iGroggyValue = 99;
				m_tGroggyManage.fGroggyStayTime = 0.f;
				m_tStatus.isGroggyStay = false;
			}
		}
		else
		{
			m_tGroggyManage.fGroggyDecreaseTime += dt;

			if (0.1f <= m_tGroggyManage.fGroggyDecreaseTime)
			{
				--m_tStatus.iGroggyValue;
				m_tGroggyManage.fGroggyDecreaseTime = 0.f;
			}

			if (0 > m_tStatus.iGroggyValue)
			{
				m_tStatus.iGroggyValue = 0;
				m_tStatus.isGroggy = false;
			}
		}
	}
}

DIR CEnemy::GetDIRToPlayer()
{
	//_vector vSelfLook = m_tTargetingInfo.vDirSelfLook;
	//vSelfLook = XMVectorSetY(vSelfLook, 0.f);
	//_vector vSelfPos = m_tTargetingInfo.vSelfPos;
	//vSelfPos = XMVectorSetY(vSelfPos, 0.f);
	//_vector vPlayerPos = m_tTargetingInfo.vTargetPos;
	//vPlayerPos = XMVectorSetY(vPlayerPos, 0.f);
	//_vector vTo = m_tTargetingInfo.vDirToTarget;
	//vTo = XMVectorSetY(vTo, 0.f);

	// 너무 가까우면 정면으로 간주
	//if (XMVectorGetX(XMVector3LengthSq(vTo)) < 1e-8f)
	//	return DIR::F;

	//vTo = XMVector3Normalize(vTo);

	//_vector vForward = XMVector3Normalize(vSelfLook);

	//_float fDot = XMVectorGetX(XMVector3Dot(vForward, vTo))

	_float fwdX = XMVectorGetX(m_tTargetingInfo.vDirSelfLook);
	_float fwdZ = XMVectorGetZ(m_tTargetingInfo.vDirSelfLook);
	_float targetX = XMVectorGetX(m_tTargetingInfo.vDirToTarget);
	_float targetZ = XMVectorGetZ(m_tTargetingInfo.vDirToTarget);

	_float fDet = fwdX * targetZ - fwdZ * targetX;
	_float fAngleRad = -atan2(fDet, m_tTargetingInfo.fDotTarget);

	_float fDegree = XMConvertToDegrees(fAngleRad);
	fDegree = fmodf(fDegree + 360.f, 360.f);

	_int isector = static_cast<_int>((fDegree + 22.5f) / 45.f) & 7;

	const DIR map[8] = { DIR::F, DIR::FR, DIR::R, DIR::BR, DIR::B, DIR::BL, DIR::L, DIR::FL };
	return map[isector];
}

void CEnemy::Parried()
{
	if (false == m_isParryEnable)
		return;

	m_tStatus.iGroggyValue += 15.f;
}

void CEnemy::UnleashAttack(ATTACK_SIDE eSide, _bool ParryEnable)
{
	SetOnAttack(true, eSide);
	Active_AttackSign(ParryEnable);
}

void CEnemy::Create_MeshPyramid()
{
	auto meshPyramid = Builder::Create_Object({ G_GlobalLevelKey, "Proto_GameObject_MeshPyramid" })
		.Build("MeshPyramid");

	Get_Component<CObjectContainer>()->Add_Child(meshPyramid);
}


void CEnemy::SetBattleColliderObject(const string& tagBattleColliderObject, BATTLE_COLTYPE eBattleColliderType, _bool is, const HitDesc& hitdesc)
{
	string tagBattleCol = tagBattleColliderObject;

	if (BATTLE_COLTYPE::ATTACK == eBattleColliderType)
		tagBattleCol += "_AttackCollider";
	//else
	//	tagBattleCol += "_TriggerCollider";

	auto iter = m_BattleColliderChildrenIndex.find(tagBattleCol);
	if (iter == m_BattleColliderChildrenIndex.end())
	{
		// 트리거 중에 해당되는 이름의 Trigger가 없을경우(만들 때, Separate == false로 했을 때)
		//if (BATTLE_COLTYPE::TRIGGER == eBattleColliderType)
		//{
		//	string defaultTriggerCol = "DefaultTriggerCollider";
		//	iter = m_BattleColliderChildrenIndex.find(defaultTriggerCol);
		//	if (iter == m_BattleColliderChildrenIndex.end())
		//		return;
		//}
		//else
			return;
	}


	auto pBattleCol = Get_Component<CObjectContainer>()->Get_Children()[iter->second];
	if (nullptr == pBattleCol)
		return;

	if (BATTLE_COLTYPE::ATTACK == eBattleColliderType)
	{
		if (true == is)
			dynamic_cast<CEnemyAttackCollider*>(pBattleCol)->Begin_Attack(hitdesc);
		else
			dynamic_cast<CEnemyAttackCollider*>(pBattleCol)->End_Attack();
	}
	pBattleCol->Get_Component<CCollider>()->Set_CompActive(is);
}

void CEnemy::FinishBattleColliderObject(const string& tagBattleColliderObject)
{
	SetBattleColliderObject(tagBattleColliderObject, BATTLE_COLTYPE::ATTACK, false);
	//SetBattleColliderObject(tagBattleColliderObject, BATTLE_COLTYPE::TRIGGER, false);
}

void CEnemy::SetAutoPlayBattleCollider(const string& tagBattleCollider, _float fAttackOffsetTime, _float fAttackPlayTime, const HitDesc& hitDesc)
{
	//SetBattleColliderObject(tagBattleCollider, BATTLE_COLTYPE::TRIGGER, true, hitDesc);

	m_tAutoBattleCol.tHitDesc = hitDesc;
	m_tAutoBattleCol.tagBattleCollider = tagBattleCollider;
	m_tAutoBattleCol.isAutoPlay = true;
	m_tAutoBattleCol.isAttackColliderPlay = false;
	m_tAutoBattleCol.fAttackColStartProgress = fAttackOffsetTime;
	m_tAutoBattleCol.vAttackColLifeTime = { fAttackPlayTime, 0.f };
}

void CEnemy::Death()
{
	if (BattleSystem()->ExitBattleObject(CBattleSystem::BATTLE_OBJ_TYPE::MONSTER, this->Get_Handle()))
	{
		ObjectManager()->Remove_Object(this);
#ifdef _USING_GUI
		auto pSelectedObject = GUISystem()->Get_Context()->pSelectedObject;
		if (nullptr != pSelectedObject &&
			this == pSelectedObject)
			GUISystem()->Get_Context()->pSelectedObject = nullptr;
#endif // _USING_GUI

		if (true == m_hUIEnemyStatus.isValid())
			UIManager()->Remove_UIObject(m_hUIEnemyStatus.Get());
	}
}

void CEnemy::SetOnAttack(_bool is, ATTACK_SIDE eSide)
{
	m_isOnAttack = is;
	m_isParryEnable = is;

	// 공격이 끝났을 때,
	if (false == is)
	{
		m_eCurAttackSide = ATTACK_SIDE::NONE;
	}
}

void CEnemy::ShowBattleColliderForCheck(_bool is)
{
	auto pContainerCom = Get_Component<CObjectContainer>();
	if (nullptr != pContainerCom)
		for (auto& PAIR : m_BattleColliderChildrenIndex) {
			auto pColliderObject = pContainerCom->Get_ChildByOrder(PAIR.second);
			if (nullptr == pColliderObject)
				continue;

			pColliderObject->Get_Component<CCollider>()->Set_CompActive(is);
		}
}

void CEnemy::CheckAutoBattlePlay(const _float dt)
{
	if (false == m_tAutoBattleCol.isAutoPlay)
		return;

	auto pAnimator3D = Get_Component<CAnimator3D>();
	if (false == m_tAutoBattleCol.isAttackColliderPlay)
	{
		if (m_tAutoBattleCol.fAttackColStartProgress <= pAnimator3D->Get_CurAnimDuration())
		{
			//SetBattleColliderObject(m_tAutoBattleCol.tagBattleCollider, BATTLE_COLTYPE::TRIGGER, false, {});
			SetBattleColliderObject(m_tAutoBattleCol.tagBattleCollider, BATTLE_COLTYPE::ATTACK, true, m_tAutoBattleCol.tHitDesc);

			m_tAutoBattleCol.isAttackColliderPlay = true;
		}
	}
	else
	{
		m_tAutoBattleCol.vAttackColLifeTime.y = pAnimator3D->Get_CurAnimDuration() - m_tAutoBattleCol.fAttackColStartProgress;

		if (true == m_tAutoBattleCol.IsAttackColFinish())
		{
			FinishBattleColliderObject(m_tAutoBattleCol.tagBattleCollider);
			m_tAutoBattleCol.vAttackColLifeTime.y = 0.f;
			m_tAutoBattleCol.isAttackColliderPlay = false;
			m_tAutoBattleCol.isAutoPlay = false;
		}
	}
}

_bool CEnemy::IsAliveBattleColliderObject(const string& tagBattleColliderObject, BATTLE_COLTYPE eBattleColliderType)
{
	string tagBattleCol = tagBattleColliderObject;

	if (BATTLE_COLTYPE::ATTACK == eBattleColliderType)
		tagBattleCol += "_AttackCollider";
	//else
		//tagBattleCol += "_TriggerCollider";

	auto iter = m_BattleColliderChildrenIndex.find(tagBattleCol);
	if (iter == m_BattleColliderChildrenIndex.end())
		return false;

	auto pBattleCol = Get_Component<CObjectContainer>()->Get_Children()[iter->second];
	if (nullptr == pBattleCol)
		return false;

	return pBattleCol->Get_Component<CCollider>()->Get_CompActive();
}


void CEnemy::Free()
{
	__super::Free();
}
