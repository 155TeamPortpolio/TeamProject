#pragma once
#include "LevelObject.h"
#include "Zero_Level.h"

NS_BEGIN(Client)
class CStageRouter :
    public CLevelObject
{
  
private:
	CStageRouter();
	~CStageRouter() DEFAULT;

public:
    virtual HRESULT Initialize_Prototype()      override { __super::Initialize_Prototype(); return S_OK; };
    virtual HRESULT Initialize(INIT_DESC* pArg) override    { __super::Initialize(pArg); return S_OK;};
    virtual void    Awake()                     override    {}
    virtual void    Priority_Update(_float dt)  override    {};
    virtual void    Update(_float dt)           override    {};
    virtual void    Late_Update(_float dt)      override    {};

public:
    virtual void    Render_GUI() override;

public:
    /*NodeControl*/
    _int AddNode(StageType type, _int parentIndex, _int depth);
    _int CreateNode(StageType type, _int parentIndex);
    void BuildGraph(_int MaxDepth, StageType root);
    _int GetChoiceNodeIndex(_int choiceIndex);
    _int GetChoiceCount();

    _bool Choose(_int choiceIndex);
    StageType GetChoiceType(_int choiceIndex);
    StageType GetCurrentType();
private:
    /*Rand*/
    _int RollChildCount(_int depth, _int maxDepth);
    StageType RollType(_int depth, _int maxDepth);

    /*Data*/

private:
    vector<StageNode> m_stageNodes;
    queue<_int> m_visitQueue;

    _int m_currentNode = -1;
    _int m_maxDepth = 0;
    _int m_guiSelectedNode = -1;
    _int m_guiHoveredNode = -1;
public:
	static CStageRouter* Create();
	virtual CGameObject* Clone(INIT_DESC* pArg) override;
	virtual void Free();
};

NS_END;