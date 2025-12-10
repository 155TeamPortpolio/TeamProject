#include "Data_Packets.h"
#include "Material.h"
#include "MaterialData.h"

RENDERKEY Engine::DrawOpaquePacket::GetKey() const
{
		return {
			pMaterial->Get_ShaderID(MaterialIndex),
			pMaterial->Get_MaterialDataID(DrawIndex)
		};
}

BLENDRENDERKEY Engine::DrawBlendedPacket::GetKey() const
{
	return {
		pMaterial->Get_ShaderID(MaterialIndex),
		pMaterial->Get_MaterialDataID(DrawIndex),
		DistanceToCamera
	};
}
