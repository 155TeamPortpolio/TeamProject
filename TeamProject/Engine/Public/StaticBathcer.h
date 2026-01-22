#pragma once
#include "Engine_Defines.h"
NS_BEGIN(Engine)
struct CellKey
{
	_int x = 0;
	_int z = 0;

	_bool operator==(const CellKey& other) const { return x == other.x && z == other.z; }
};

struct CellKeyHash
{
	size_t operator()(const CellKey& key) const
	{
		return (size_t)key.x * 73856093u ^ (size_t)key.z * 19349663u;
	}
};

struct BatchingKey
{
	class CMaterial* pMaterial = { nullptr };
	_uint iMaterialIndex = {};
	class CShader* pShader = { nullptr };

	_bool operator==(const BatchingKey& rhs) const {
		return pMaterial == rhs.pMaterial &&
			pShader == rhs.pShader &&
			iMaterialIndex == rhs.iMaterialIndex;
	}
};


struct BatchingKeyHash
{
	size_t operator()(const BatchingKey& key) const
	{
		size_t hashValue = (size_t)key.pMaterial;
		hashValue ^= (size_t)key.iMaterialIndex * 16777619u;
		hashValue ^= (size_t)key.iMaterialIndex * 2166136261u;
		return hashValue;
	}
};


struct CellBatchKey
{
	CellKey cell;
	BatchingKey batch;

	_bool operator==(const CellBatchKey& other) const
	{
		return cell == other.cell && batch == other.batch;
	}
};

struct CellBatchKeyHash
{
	size_t operator()(const CellBatchKey& key) const
	{
		CellKeyHash cellHasher;
		BatchingKeyHash batchHasher;
		return cellHasher(key.cell) ^ (batchHasher(key.batch) << 1);
	}
};

struct CachedBatch
{
	ID3D11Buffer* pVB = nullptr;
	ID3D11Buffer* pIB = nullptr;

	_uint vertexStride = sizeof(VTXMESH);
	_uint indexCount = 0;

	MINMAX_BOX worldAabb = {};

	class CMaterial* material = { nullptr };
	_uint materialIndex = 0;
	class  CShader* shader = { nullptr };
	
	_uint lastUsedFrame = 0;
};

NS_END