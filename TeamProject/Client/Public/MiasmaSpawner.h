#pragma once
NS_BEGIN(Client)
enum class MiasmaType {Heavy, Grandier, Blade};
class CMiasmaSpawner
{
public:
	void Spawn(MiasmaType type, _int count, _float3 Target, _float3 Owner, _float y, class CDefiler* pDefiler);
	void SpawnGrandier(_int count, _float3 Target, _float3 Owner, _float y);
	void SpawnBlade( _float3 Target, _float3 Owner, class CDefiler* pDefiler);
};

NS_END
