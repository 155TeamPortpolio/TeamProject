#pragma once
#include "IPhysicsService.h"

NS_BEGIN(Engine)

class CPhysicsSystem final : public IPhysicsService
{
#ifdef USINGPHYSICS 

private:
    CPhysicsSystem() DEFAULT;
    virtual ~CPhysicsSystem() DEFAULT;

public:
    PxPhysics*           Get_Physics() override { return m_pPhysics; }
    PxScene*             Get_Scene() override { return m_pScene; }
    PxControllerManager* Get_ControllerManager() override { return m_pControllerManager; }
    PxMaterial*          Get_DefaultMaterial() override { return m_pMaterial; }
    HRESULT              Add_Material(const string& strKey, _float fStatic, _float fDynamic, _float fRestitution) override;
    PxMaterial*          Get_Material(const string& strKey) override;

public:
    HRESULT Initialize();
    virtual void Update(_float dt) override;
    virtual void Late_Update(_float dt) override;
    static PxFilterFlags SimulationFilterShader(
        PxFilterObjectAttributes attributes0, PxFilterData filterData0,
        PxFilterObjectAttributes attributes1, PxFilterData filterData1,
        PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);

private:
    class CUserErrorCallback : public PxErrorCallback
    {
    public:
        virtual void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line) override
        {
            // 디버그 출력
            char buffer[1024];
            sprintf_s(buffer, "[PhysX Error] %s (%s line %d)\n", message, file, line);
            OutputDebugStringA(buffer);
        }
    };

private:
    PxFoundation*            m_pFoundation = { nullptr };            // 기반
    PxPhysics*               m_pPhysics = { nullptr };               // 물리객체를 만들기(Device 역할)
    PxPvd*                   m_pPvd = { nullptr };                   // 디버깅 툴용
    PxDefaultCpuDispatcher*  m_pDispatcher = { nullptr };            // 스레드관리자(멀티스레딩 물리연산)
    PxScene*                 m_pScene = { nullptr };                 // 물리월드
    PxMaterial*              m_pMaterial = { nullptr };              // 기본 물리재질
    map<string, PxMaterial*> m_Materials;                            // 물리재질
    PxControllerManager*     m_pControllerManager = { nullptr };     // 캐릭터 움직임 관리하는 매니저
    PxDefaultAllocator       m_Allocator;                            // 메모리 할당자
    CUserErrorCallback       m_ErrorCallback;                        // 에러 리포터

    _float                  m_fTimer = {};
    const _float            m_fDelta = 1.0f / 60.f;

public:
    static CPhysicsSystem* Create();
    virtual void Free() override;
#endif

};

NS_END