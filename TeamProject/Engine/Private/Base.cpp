#include "Engine_Defines.h"
#include "Base.h"

_uint CBase::AddRef()
{
	return ++m_iRefCnt;
}

_uint CBase::Release()
{

	if (m_iRefCnt == 0)
	{
		Free();
		delete this;
		return 0;
	}

	else {
		return m_iRefCnt--;
	}
}

void CBase::Free()
{
}
