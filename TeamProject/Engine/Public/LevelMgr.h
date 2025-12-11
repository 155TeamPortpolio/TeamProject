#pragma once
#include "ILevelService.h"

NS_BEGIN(Engine)

class CLevelMgr :
    public ILevelService
{
private:
    CLevelMgr();
    virtual ~CLevelMgr();

public: 
    HRESULT Initialize();
    virtual  HRESULT Request_ChangeLevel(string levelID, _bool Load = true) override;/*레벨 체인지 요청*/
    virtual void Update(_float dt)override;
    virtual HRESULT Render(ID3D11DeviceContext* pContext)override;

public :
    virtual void Register_Level(const string& levelID, LEVEL_CREATOR creator)override; /*레벨 생성자 등록*/

#pragma region For_OtherManager
    virtual _uint Get_LevelCount() override { return m_LevelCreators.size(); }
    virtual const vector<string> Get_LevelList()override; //레벨 키 모음
    virtual _bool Check_ValidateLevel(const string& LevelTag)override;
    virtual const string& Get_NowLevelKey()override;
    virtual  class CLevel* Get_CurrentLevel()override { return m_pCurrentLevel; };
#pragma endregion

#pragma region For_LoadingLevel
    virtual void Set_LoadingLevel(const string& LoadingKey)override; //로딩 역할을 하는 레벨이 있는지.
    virtual const string& Get_NextLevel() override { return m_NextLevelTag; } //로딩 이후 레벨은 무엇인지
    virtual void Notify_LoadComplete()override; //로딩이 다되었다면 호출
#pragma endregion
private:
    void ClearResource();

private:
    string m_LoadingLevelKey = { }; // 로딩 레벨이 있다면 그 로딩 레벨의 태그(키)

    class CLevel* m_pCurrentLevel = { nullptr }; //현재 레벨 포인터
    class CLevel* m_pReadyLevel = { nullptr };      //대기중 레벨 포인터
    LEVEL_STATE m_eState = {}; //현재 레벨 전환 과정 상태
    string m_NextLevelTag = { }; //전환될 레벨 태그(키)
    
    unordered_map<string, LEVEL_CREATOR> m_LevelCreators;/*레벨 생성자*/
public:
    static CLevelMgr* Create();
    virtual void Free() override;
};

NS_END