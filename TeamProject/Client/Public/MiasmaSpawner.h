#pragma once
NS_BEGIN(Client)
enum class MiasmaType {Heavy, Grandier};
class CMiasmaSpawner
{
public:
	void Spawn(MiasmaType type, _int count, _float3 Target, _float3 Owner, _float y);
};

NS_END
