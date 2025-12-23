#include "Engine_Defines.h"
#include "ResourceEntry.h"

CResourceEntry::CResourceEntry()
{
}

CResourceEntry::~CResourceEntry()
{
}

void CResourceEntry::Release_NoLock()
{
    /*Variant 안에 값을 꺼내서 람다에 넣는 함수*/
   visit([&](auto&& value)
        {
           /*타입을 꺼내서 저장*/
            using Value = decay_t<decltype(value)>;
            /*포인터라면*/
            if constexpr (is_pointer_v<Value>)
            {
                Safe_Release(value);   
            }
        }, m_Resource);

    m_Resource = monostate{};
}

CResourceEntry* CResourceEntry::Create() {
	return new CResourceEntry;
}

void CResourceEntry::Free() {
	__super::Free();
    Release_NoLock();
}