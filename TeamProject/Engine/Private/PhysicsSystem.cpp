#pragma once
#include "Engine_Defines.h"
#include "PhysicsSystem.h"

#include "GameInstance.h"
#ifdef USINPHYSICS 

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
    sceneDesc.filterShader = PxDefaultSimulationFilterShader; // 기본 충돌 필터

#ifdef _DEBUG
    // 디버그 모드일 때 씬 정보를 PVD로 전송
    if (m_pPvd->isConnected())
    {
        // PvdSceneClient 플래그 설정
        sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;
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
#endif

    m_pControllerManager = PxCreateControllerManager(*m_pScene);     // Controller Manager 생성
    // 기본 재질 생성
    m_pMaterial = m_pPhysics->createMaterial(0.5f, 0.5f, 0.6f); // 정지마찰, 운동마찰, 반발계수

    return S_OK;
}

void CPhysicsSystem::Update(_float dt)
{
    if (!m_pScene) return;

    m_fTimer += dt;

    int iMaxSteps = 5; // 무한 루프 방지용 최대 5회 제한

    while (m_fTimer >= m_fDelta)
    {
        if (iMaxSteps-- <= 0)
        {
            m_fTimer = 0.f;
            break;
        }
        m_pScene->simulate(m_fDelta);
        m_pScene->fetchResults(true);
        m_fTimer -= m_fDelta;
    }
}

void CPhysicsSystem::Late_Update(_float dt)
{
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
    for (auto& pair : m_Materials)
        pair.second->release();
    m_Materials.clear();

    if (m_pMaterial)
    {
        m_pMaterial->release();
        m_pMaterial = nullptr;
    }

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