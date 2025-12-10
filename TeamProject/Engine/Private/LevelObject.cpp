#include "LevelObject.h"

CLevelObject::CLevelObject()
{
}

CLevelObject::CLevelObject(const CLevelObject& rhs)
	:CGameObject(rhs)
{
}

void CLevelObject::Free()
{
	__super::Free();
}
