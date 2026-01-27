#pragma once
#include "Enemy.h"

NS_BEGIN(Client)

template<typename Type>
class CStateMachine;

class CDefiler :
    public CEnemy
{
public:
    typedef struct tagDefilerBlackBoard : public ATTACK_BLACK_BOARD
    {
        //  deque<string> stateQueue;
        //  _bool isRequestNext = false;//다음 상태가 존재 할 때 상태 전환 요청
        //  _bool isChainOpen = false;  //현재 상태에서 다음으로 진행 가능여부
        //  _bool isEnd = false;
        //  string currentStateTag{};

        _bool TraceOn = {};
        deque<_float> progressQueue; 
        vector<string> FrameEffect;
        _bool EndChain = {};

        _float GetProgress() {
            if (progressQueue.empty())
                return 0.f;
            else {
                _float progress = progressQueue.front();
                progressQueue.pop_front();
                return progress;
            }
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
    void MoveByRootMotion(_float dt, _float moveScale = 1.f);
    void RotateToTarget(_float dt, _float rotateSpeed = 1.f);
    void Update_States(_float dt);
    void Route_AnimEvent(CAnimator3D* animator);

private:
    HRESULT Initialize_StateMachine();
    HRESULT Initialize_States();
    HRESULT Initialize_Transitions();
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
