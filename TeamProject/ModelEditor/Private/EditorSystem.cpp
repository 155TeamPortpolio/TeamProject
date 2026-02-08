#include "pch.h"
#include "EditorSystem.h"
#include "GameInstance.h"
#include "EditModel.h"
#include "RayCaster.h"
#include "Layer.h"
#include "AI_SKModel.h"
#include "AI_STModel.h"
#include "AIModelData.h"
#include "AIMesh.h"
#include "GameObject.h"
#include "ModelHelper.h"
#include "Helper_Func.h"

IMPLEMENT_SINGLETON(CEditorSystem)

CEditorSystem::CEditorSystem()
{
}

HRESULT CEditorSystem::Initialize()
{
    m_CheckNameRules.emplace("proxy");
    m_CheckNameRules.emplace("dummy");

	return S_OK;
}

void CEditorSystem::Update()
{
	
}


_bool CEditorSystem::CheckNamingRule(string MeshName)
{
    string lowerName = Helper::ToLower(MeshName);
    for (auto& name : m_CheckNameRules)
    {
        if (lowerName.find(name) != string::npos)
            return false;
    }
    return true;
}

unordered_set<string> CEditorSystem::Get_NamingRules()
{
    return m_CheckNameRules;
}

void CEditorSystem::Push_NamingRules(string MeshName)
{
    m_CheckNameRules.emplace(MeshName);
}

void CEditorSystem::Pop_NamingRules(string MeshName)
{
    m_CheckNameRules.erase(MeshName);
}


void CEditorSystem::Free()
{
    __super::Free();
}
