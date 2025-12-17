#pragma once
#include "Engine_Defines.h"
#include "PhysicsSystem.h"

#include "GameInstance.h"
#include "CharacterController.h"

#include "StaticModel.h"
#include "ModelData.h"
#include "Mesh.h"

#ifdef USINGPHYSICS 

HRESULT CPhysicsSystem::Add_Material(const string& strKey, _float fStatic, _float fDynamic, _float fRestitution)
{
    if (m_Materials.find(strKey) != m_Materials.end())
        return E_FAIL;  // already  exist

    PxMaterial* pMaterial = m_pPhysics->createMaterial(fStatic, fDynamic, fRestitution);
    if (!pMaterial) return E_FAIL;  // failed to create material

    m_Materials.emplace(strKey, pMaterial);
    return S_OK;
}

PxMaterial* CPhysicsSystem::Get_Material(const string& strKey)
{
    auto iter = m_Materials.find(strKey);
    if (iter == m_Materials.end())
        return m_pMaterial; // 없으면 기본 재질 반환

    return iter->second;
}

HRESULT CPhysicsSystem::Initialize()
{
    // Foundation 생성
    m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);
    if (!m_pFoundation) return E_FAIL;

    // PVD (Visual Debugger) 설정
#ifdef _DEBUG 
    // PVD 생성
    m_pPvd = PxCreatePvd(*m_pFoundation);
    // PVD 연결 (로컬호스트, 포트 5425, 타임아웃 10ms)
    PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
    m_pPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
    if (m_pPvd->isConnected())
    {
        // 연결 성공 로그
        OutputDebugStringA("------------------------------------------------\n");
        OutputDebugStringA("   [PhysX] PVD Connected Successfully!          \n");
        OutputDebugStringA("------------------------------------------------\n");
    }
    else
    {
        // 연결 실패 로그
        OutputDebugStringA("------------------------------------------------\n");
        OutputDebugStringA("   [PhysX] PVD Connection Failed...             \n");
        OutputDebugStringA("------------------------------------------------\n");
    }
#else
    m_pPvd = nullptr; // 릴리즈 모드
#endif

    // Physics 생성 (PVD 연결)
    m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true, m_pPvd);
    if (!m_pPhysics) return E_FAIL;
    if (!PxInitExtensions(*m_pPhysics, m_pPvd)) return E_FAIL;   // Extensions 초기화 (필수적인 확장 기능들)

    m_pDispatcher = PxDefaultCpuDispatcherCreate(2);             // Dispatcher 생성 (CPU 스레드 2개 사용)

    // Scene(물리 월드) 생성
    PxSceneDesc sceneDesc(m_pPhysics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f); // 중력 설정
    sceneDesc.cpuDispatcher = m_pDispatcher;
    sceneDesc.filterShader = SimulationFilterShader; // 기본 충돌 필터
    sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;
    sceneDesc.broadPhaseType = PxBroadPhaseType::eSAP;
    sceneDesc.flags |= PxSceneFlag::eENABLE_STABILIZATION;
    sceneDesc.ccdMaxPasses = 4;        // 기본값 1 -> 4로 증가
    sceneDesc.bounceThresholdVelocity = 0.2f * 9.81f;  // 중력 기반
#ifdef _DEBUG
    // 디버그 모드일 때 씬 정보를 PVD로 전송
    if (m_pPvd->isConnected())
    {
        // PvdSceneClient 플래그 설정
        sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;
        sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVE_ACTORS;

        // (주의: PhysX 버전에 따라 sceneDesc.flags가 아니라 scene->getPvdSceneClient()->setScenePvdFlag 사용이 필요할 수 있음)
    }
#endif

    m_pScene = m_pPhysics->createScene(sceneDesc);
    if (!m_pScene) return E_FAIL;

#ifdef _DEBUG
    // Scene의 PVD 플래그
    PxPvdSceneClient* pvdClient = m_pScene->getScenePvdClient();
    if (pvdClient)
    {
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
    }
    m_pScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
    m_pScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
#endif

    m_pControllerManager = PxCreateControllerManager(*m_pScene);     // Controller Manager 생성
    
    // 머터리얼 데이터 세팅 : 필요시 추가
    Add_Material("Default", 0.5f, 0.5f, 0.6f);
    m_pMaterial = Get_Material("Default");

    // Cooking 초기화
    PxCookingParams cookingParams(m_pPhysics->getTolerancesScale());
    cookingParams.meshPreprocessParams |= PxMeshPreprocessingFlag::eWELD_VERTICES;
    cookingParams.meshWeldTolerance = 0.001f;
    m_pCooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_pFoundation, cookingParams);
    if (!m_pCooking)
        return E_FAIL;

    return S_OK;
}

void CPhysicsSystem::Update(_float dt)
{
    if (!m_pScene) return;

    m_pScene->simulate(dt);

}

void CPhysicsSystem::Late_Update(_float dt)
{
    m_pScene->fetchResults(true);
}

_bool CPhysicsSystem::Raycast(const PHYSICS_RAY& desc, PHYSICS_RAY_HIT& outHit)
{
    if (!m_pScene) return false;

    PxVec3 origin(desc.vOrigin.x, desc.vOrigin.y, desc.vOrigin.z);
    PxVec3 direction(desc.vDirection.x, desc.vDirection.y, desc.vDirection.z);
    direction.normalize();

    PxRaycastBuffer hit;
    PxQueryFilterData filterData = Create_FilterData(desc);

    _bool bResult = m_pScene->raycast(origin, direction, desc.fMaxDistance, hit,
        PxHitFlag::eDEFAULT, filterData);

    if (bResult && hit.hasBlock)
    {
        Setup_RayHitInfo(hit.block, outHit);
        if (!desc.bQueryTrigger && outHit.pCollidable)
        {
            CCollider* pCollider = dynamic_cast<CCollider*>(outHit.pCollidable);
            if (pCollider && pCollider->IsTrigger())
            {
                // 트리거는 무시하고 다음 검사 (구현 복잡)
                return false;
            }
        }
        return true;
    }

    return false;
}

_bool CPhysicsSystem::Raycast_Multiple(const PHYSICS_RAY& desc, PHYSICS_RAY_HITS& outHits)
{
    if (!m_pScene) return false;

    outHits.Clear();

    PxVec3 origin(desc.vOrigin.x, desc.vOrigin.y, desc.vOrigin.z);
    PxVec3 direction(desc.vDirection.x, desc.vDirection.y, desc.vDirection.z);
    direction.normalize();

    const PxU32 bufferSize = desc.iMaxHits > 0 ? desc.iMaxHits : 128;
    PxRaycastHit* hitBuffer = new PxRaycastHit[bufferSize];
    PxRaycastBuffer hit(hitBuffer, bufferSize);
    PxQueryFilterData filterData = Create_FilterData(desc);

    _bool bResult = m_pScene->raycast(origin, direction, desc.fMaxDistance, hit,
        PxHitFlag::eDEFAULT, filterData);

    if (bResult)
    {
        outHits.iHitCount = hit.nbTouches;
        outHits.vecHits.reserve(hit.nbTouches);

        for (PxU32 i = 0; i < hit.nbTouches; ++i)
        {
            PHYSICS_RAY_HIT hitInfo;
            Setup_RayHitInfo(hit.touches[i], hitInfo);
            outHits.vecHits.push_back(hitInfo);
        }

        if (hit.hasBlock)
        {
            PHYSICS_RAY_HIT hitInfo;
            Setup_RayHitInfo(hit.block, hitInfo);
            outHits.vecHits.push_back(hitInfo);
            outHits.iHitCount++;
        }
    }

    delete[] hitBuffer;
    return bResult;
}

_bool CPhysicsSystem::Raycast_All(const PHYSICS_RAY& desc, PHYSICS_RAY_HITS& outHits)
{
    PHYSICS_RAY allDesc = desc;
    allDesc.iMaxHits = 128;
    return Raycast_Multiple(allDesc, outHits);
}

PxTriangleMesh* CPhysicsSystem::Cook_TriangleMesh(const string& strModelKey, CModel* pModel)
{
    if (!pModel || !m_pCooking)
        return nullptr;

    // 캐시 확인
    auto iter = m_CachedTriangleMeshes.find(strModelKey);
    if (iter != m_CachedTriangleMeshes.end())
        return iter->second;

    // 쿠킹된 파일 확인
    string strCookedPath = "../../DemoResource/Physics/Cooked/" + strModelKey + ".px";

    PxTriangleMesh* pTriMesh = nullptr;

    if (filesystem::exists(strCookedPath))
    {
        pTriMesh = Load_CookedMesh(strCookedPath);
    }
    else
    {
        if (FAILED(Cooking(strModelKey, pModel)))
            return nullptr;

        pTriMesh = Load_CookedMesh(strCookedPath);
    }

    if (pTriMesh)
        m_CachedTriangleMeshes[strModelKey] = pTriMesh;

    return pTriMesh;
}

void CPhysicsSystem::Setup_RayHitInfo(const PxRaycastHit& pxHit, PHYSICS_RAY_HIT& outHit)
{
    outHit.bHit = true;
    outHit.fDistance = pxHit.distance;
    outHit.vPoint = _float3(pxHit.position.x, pxHit.position.y, pxHit.position.z);
    outHit.vNormal = _float3(pxHit.normal.x, pxHit.normal.y, pxHit.normal.z);
    outHit.pShape = pxHit.shape;

    if (pxHit.actor && pxHit.actor->userData)
    {
        outHit.pHitObject = static_cast<CGameObject*>(pxHit.actor->userData);

        if (pxHit.shape && pxHit.shape->userData)
        {
            outHit.pCollidable = static_cast<ICollidable*>(pxHit.shape->userData);
        }
        else if (outHit.pHitObject)
        {
            outHit.pCollidable = outHit.pHitObject->Get_Component<CCollider>();
            if (!outHit.pCollidable)
                outHit.pCollidable = outHit.pHitObject->Get_Component<CCharacterController>();
        }
    }
}

PxQueryFilterData CPhysicsSystem::Create_FilterData(const PHYSICS_RAY& desc)
{
    PxQueryFilterData filterData;
    filterData.data.word0 = desc.iCollisionMask;
    filterData.flags = PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC |
        PxQueryFlag::ePREFILTER;
    return filterData;
}

PxFilterFlags CPhysicsSystem::SimulationFilterShader(
    PxFilterObjectAttributes attributes0, PxFilterData filterData0,
    PxFilterObjectAttributes attributes1, PxFilterData filterData1,
    PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
    // 트리거(Trigger)인 경우
    if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
    {
        pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
        return PxFilterFlag::eDEFAULT;
    }
    // 물리적 충돌 처리 + 알림 켜기
    // eCONTACT_DEFAULT: 물리적으로 튕겨내라 (이게 있어야 안 뚫립니다!)
    // eNOTIFY_TOUCH_FOUND: 충돌 시작되면 onContact 호출해라
    pairFlags = PxPairFlag::eCONTACT_DEFAULT
        | PxPairFlag::eSOLVE_CONTACT         // 물리적 반발력
        | PxPairFlag::eDETECT_DISCRETE_CONTACT // 일반 충돌 감지
        | PxPairFlag::eDETECT_CCD_CONTACT    // CCD 충돌 감지 허용
        | PxPairFlag::eNOTIFY_TOUCH_FOUND
        | PxPairFlag::eNOTIFY_TOUCH_LOST;

    return PxFilterFlag::eDEFAULT;
}

HRESULT CPhysicsSystem::Cooking(const string& strModelKey, CModel* pModel)
{
    if (!m_pCooking || !pModel)
        return E_FAIL;

    CModelData* pModelData = static_cast<CStaticModel*>(pModel)->Get_ModelData();
    if (!pModelData)
        return E_FAIL;

    vector<PxVec3> vertices;
    vector<PxU32> indices;

    _uint iMeshCount = pModelData->Get_MeshCount();

#ifdef _DEBUG
    char debugMsg[512];
    sprintf_s(debugMsg, "[PhysX Cooking] Start: %s (Meshes: %d)\n", strModelKey.c_str(), iMeshCount);
    OutputDebugStringA(debugMsg);
#endif

    _uint skippedMeshCount = 0;

    for (_uint i = 0; i < iMeshCount; ++i)
    {
        CMesh* pMesh = pModelData->Get_Mesh(i);

        // 필터링 : 메쉬 이름 체크
        string meshName = pMesh->Get_Key();

        _bool bShouldCook = false;

        if (meshName.find("_Proxy") != string::npos)
        {
            // _Proxy 메쉬만 쿠킹
            bShouldCook = true;
        }
        if (!bShouldCook)
        {
#ifdef _DEBUG
            sprintf_s(debugMsg, "[PhysX Cooking] Skip Mesh %d: '%s' (not Proxy)\n",
                i, meshName.c_str());
            OutputDebugStringA(debugMsg);
#endif
            skippedMeshCount++;
            continue;
        }

        const vector<VTXMESH>& meshVerts = pMesh->Get_StaticVertices();
        const vector<_uint>& meshIndices = pMesh->Get_Indices();

        // 필터링 : 빈 데이터
        if (meshVerts.empty() || meshIndices.empty())
        {
#ifdef _DEBUG
            sprintf_s(debugMsg, "[PhysX Cooking] Skip Mesh %d: '%s' (empty data)\n",
                i, meshName.c_str());
            OutputDebugStringA(debugMsg);
#endif
            skippedMeshCount++;
            continue;
        }

        // 필터링 : 삼각형이 아닌 메쉬
        if (meshIndices.size() % 3 != 0)
        {
#ifdef _DEBUG
            sprintf_s(debugMsg, "[PhysX Cooking] Skip Mesh %d: '%s' (not triangles)\n",
                i, meshName.c_str());
            OutputDebugStringA(debugMsg);
#endif
            skippedMeshCount++;
            continue;
        }

        // 필터링 : 버텍스 최소 개수
        if (meshVerts.size() < 3)
        {
#ifdef _DEBUG
            sprintf_s(debugMsg, "[PhysX Cooking] Skip Mesh %d: '%s' (too few vertices)\n",
                i, meshName.c_str());
            OutputDebugStringA(debugMsg);
#endif
            skippedMeshCount++;
            continue;
        }

        // 필터링 : 인덱스 범위 검증
        _bool bValidIndices = true;
        for (_uint idx : meshIndices)
        {
            if (idx >= meshVerts.size())
            {
#ifdef _DEBUG
                sprintf_s(debugMsg, "[PhysX Cooking] Skip Mesh %d: '%s' (invalid indices)\n",
                    i, meshName.c_str());
                OutputDebugStringA(debugMsg);
#endif
                bValidIndices = false;
                break;
            }
        }

        if (!bValidIndices)
        {
            skippedMeshCount++;
            continue;
        }

        // 유효한 메쉬 추가
        _uint vertexOffset = vertices.size();

        for (const auto& vtx : meshVerts)
        {
            vertices.push_back(PxVec3(vtx.vPosition.x, vtx.vPosition.y, vtx.vPosition.z));
        }

        for (_uint idx : meshIndices)
        {
            indices.push_back(idx + vertexOffset);
        }

#ifdef _DEBUG
        _uint triangleCount = meshIndices.size() / 3;
        sprintf_s(debugMsg, "[PhysX Cooking] Mesh %d OK: '%s' - Verts=%zu, Triangles=%d\n",
            i, meshName.c_str(), meshVerts.size(), triangleCount);
        OutputDebugStringA(debugMsg);
#endif
    }

#ifdef _DEBUG
    sprintf_s(debugMsg, "[PhysX Cooking] Processed: %d valid, %d skipped\n",
        iMeshCount - skippedMeshCount, skippedMeshCount);
    OutputDebugStringA(debugMsg);
#endif

    if (vertices.empty() || indices.empty())
    {
#ifdef _DEBUG
        OutputDebugStringA("[PhysX Cooking] Error: No valid mesh data after filtering\n");
#endif
        return E_FAIL;
    }

    PxTriangleMeshDesc meshDesc;
    meshDesc.points.count = static_cast<PxU32>(vertices.size());
    meshDesc.points.stride = sizeof(PxVec3);
    meshDesc.points.data = vertices.data();

    meshDesc.triangles.count = static_cast<PxU32>(indices.size() / 3);
    meshDesc.triangles.stride = 3 * sizeof(PxU32);
    meshDesc.triangles.data = indices.data();

#ifdef _DEBUG
    sprintf_s(debugMsg, "[PhysX Cooking] Final: Vertices=%d, Triangles=%d\n",
        meshDesc.points.count, meshDesc.triangles.count);
    OutputDebugStringA(debugMsg);
#endif

    string strSavePath = "../../DemoResource/Physics/Cooked/" + strModelKey + ".px";
    filesystem::create_directories("../../DemoResource/Physics/Cooked/");

    PxDefaultFileOutputStream writeBuffer(strSavePath.c_str());
    PxTriangleMeshCookingResult::Enum result;

    if (!m_pCooking->cookTriangleMesh(meshDesc, writeBuffer, &result))
    {
#ifdef _DEBUG
        OutputDebugStringA("[PhysX Cooking] Error: Cook failed\n");
#endif
        return E_FAIL;
    }

    switch (result)
    {
    case PxTriangleMeshCookingResult::eSUCCESS:
#ifdef _DEBUG
        sprintf_s(debugMsg, "[PhysX Cooking] Success: %s\n", strModelKey.c_str());
        OutputDebugStringA(debugMsg);
#endif
        break;

    case PxTriangleMeshCookingResult::eLARGE_TRIANGLE:
#ifdef _DEBUG
        sprintf_s(debugMsg, "[PhysX Cooking] Warning: Large triangles (still usable)\n");
        OutputDebugStringA(debugMsg);
#endif
        break;

    case PxTriangleMeshCookingResult::eFAILURE:
#ifdef _DEBUG
        OutputDebugStringA("[PhysX Cooking] Error: Cook failed\n");
#endif
        return E_FAIL;
    }

    return S_OK;
}

PxTriangleMesh* CPhysicsSystem::Load_CookedMesh(const string& strFilePath)
{
    if (!m_pPhysics)
        return nullptr;

    PxDefaultFileInputData readBuffer(strFilePath.c_str());
    return m_pPhysics->createTriangleMesh(readBuffer);
}

CPhysicsSystem* CPhysicsSystem::Create()
{
    CPhysicsSystem* pInstance = new CPhysicsSystem();
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Create : CPhysicsSystem");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CPhysicsSystem::Free()
{
    for (auto& pair : m_CachedTriangleMeshes)
    {
        if (pair.second)
            pair.second->release();
    }
    m_CachedTriangleMeshes.clear();

    if (m_pCooking)
    {
        m_pCooking->release();
        m_pCooking = nullptr;
    }

    for (auto& pair : m_Materials)
        pair.second->release();
    m_Materials.clear();
    m_pMaterial = nullptr;

    if (m_pControllerManager)
    {
        m_pControllerManager->release();
        m_pControllerManager = nullptr;
    }

    if (m_pScene)
    {
        m_pScene->release();
        m_pScene = nullptr;
    }

    if (m_pDispatcher)
    {
        m_pDispatcher->release();
        m_pDispatcher = nullptr;
    }

    PxCloseExtensions();

    if (m_pPhysics)
    {
        m_pPhysics->release();
        m_pPhysics = nullptr;
    }

    if (m_pPvd)
    {
        PxPvdTransport* transport = m_pPvd->getTransport();
        m_pPvd->release();
        m_pPvd = nullptr;
        if (transport) transport->release();
    }

    if (m_pFoundation)
    {
        m_pFoundation->release();
        m_pFoundation = nullptr;
    }
}
#endif