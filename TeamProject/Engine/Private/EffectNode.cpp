#include "Engine_Defines.h"
#include "EffectNode.h"

CEffectNode::CEffectNode()
	:CGameObject()
{
}

CEffectNode::CEffectNode(const CEffectNode& rhs)
	:CGameObject(rhs)
{
}

void CEffectNode::Render_GUI()
{
	__super::Render_GUI();
}

void CEffectNode::Free()
{
	__super::Free();
}
