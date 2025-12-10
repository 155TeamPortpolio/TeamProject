#include "Child.h"
#include "GameObject.h"
//#include ""

CChild::CChild(CGameObject* pParent)
	:m_pParent{pParent}
{
	//Safe_AddRef(m_pParent);
}

CChild::CChild(const CChild& rhs)
{
}


HRESULT CChild::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CChild::Initialize(COMPONENT_DESC* pArg)
{
	if (pArg != nullptr)
	{
		Set_Parent(static_cast<PARENT_DESC*>(pArg)->pParent);
	}
	return S_OK;
}

void CChild::Sync_To_Parent(_bool SyncTransform)
{
	CTransform* myTransform = m_pOwner->Get_Component<CTransform>();
	CTransform* ParentTransform =m_pParent->Get_Component<CTransform>();

	if(SyncTransform)
		myTransform->Set_ParentTransform(ParentTransform);
}

void CChild::Set_Parent(CGameObject* pParent)
{
	m_pParent = pParent;
}

void CChild::Dettach_Parent()
{
	m_pParent = nullptr;
	CTransform* myTransform = m_pOwner->Get_Component<CTransform>();
	myTransform->Set_ParentTransform(nullptr);
}

CChild* CChild::Create(CGameObject* pParent)
{
	CChild* instance = new CChild(pParent);

	if (FAILED(instance->Initialize_Prototype())) {
		MSG_BOX("CChild Create Failed : CChild");
		Safe_Release(instance);
	}

	return instance;
}

CComponent* CChild::Clone()
{
	CChild* instance = new CChild(*this);
	return instance;
}

void CChild::Free()
{
	__super::Free();
}
