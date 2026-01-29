#pragma once
#include "Enemy.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;

class CDefiler :
    public CEnemy
{
public:
    enum class TraceType {TRACE, ONTARGET, NONE};
    typedef struct tagDefilerBlackBoard : public ATTACK_BLACK_BOARD
    {
        //  deque<string> stateQueue;
        //  _bool isRequestNext = false;//다음 상태가 존재 할 때 상태 전환 요청
        //  _bool isChainOpen = false;  //현재 상태에서 다음으로 진행 가능여부
        //  _bool isEnd = false;
        //  string currentStateTag{};
        _int pattern = { 0 };

        TraceType eTraceType = {};
        deque<_float> progressQueue; 
        _float currentStartProgress = 0.f;
        _bool EndChain = {};

        void OnTarget() { eTraceType = TraceType::ONTARGET; }
        void OnTrace() { eTraceType = TraceType::TRACE; }
        void NoneTrace() { eTraceType = TraceType::NONE; }
        _float PopStartProgress()
        {
            _float progress = 0.f;
            if (!progressQueue.empty())
            {
                progress = progressQueue.front();
                progressQueue.pop_front();
            }
            return progress;
        }
        _bool ConsumeChain() {
            _bool prevChain = EndChain;
            EndChain = false;
            return prevChain;
        }

    }DEFILER_BLACK_BOARD;

private:
    CDefiler();
    CDefiler(const CDefiler& rhg);
    virtual ~CDefiler() DEFAULT;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(INIT_DESC* pArg) override;
    void    Awake() override;
    void    Priority_Update(_float dt) override;
    void    Update(_float dt) override;
    void    Late_Update(_float dt) override;

    void    Render_GUI();

public:
    DEFILER_BLACK_BOARD& GetBlackBoard() { return m_BlackBoard; }
    CStateMachine<CDefiler>* Get_MainStateMachine() { return m_pStateMachine; }

private:
    void MoveByTraceMode(_float dt, _float moveScale = 1.f);
    void RotateToTarget(_float dt, _float rotateSpeed = 1.f);
    void Update_States(_float dt);
    void Route_AnimEvent(CAnimator3D* animator);

private:
    HRESULT Initialize_StateMachine();
    HRESULT Initialize_States();
    HRESULT Initialize_Transitions();
    HRESULT Initialize_Effects();
    //HRESULT Create_Colliders();

private:
    CStateMachine<CDefiler>* m_pStateMachine = { nullptr };
    DEFILER_BLACK_BOARD m_BlackBoard = {};

public:
    static CDefiler* Create();
    CGameObject* Clone(INIT_DESC* pArg) override;
    virtual void Free() override;
};

NS_END
