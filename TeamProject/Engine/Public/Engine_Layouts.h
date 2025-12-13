#pragma once
#include "Engine_Defines.h"

namespace Engine {

	/* Input LayOut*/
	typedef struct ENGINE_DLL tagVertexPosition {
		XMFLOAT3		vPosition;

		static constexpr string_view  Key = "VTXPOS";
		static constexpr unsigned int iElementCount = { 1 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[iElementCount] = {
			{"POSITION",        0,      DXGI_FORMAT_R32G32B32_FLOAT,         0,      0,		D3D11_INPUT_PER_VERTEX_DATA,	0},
		};
	}VTXPOS;

	typedef struct ENGINE_DLL tagVertexPositionColor {
		XMFLOAT3		vPosition;
		XMFLOAT4		vColor;

		static constexpr string_view  Key = "VTXCOL";
		static constexpr unsigned int iElementCount = { 2 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[iElementCount] = {
			{"POSITION",        0,      DXGI_FORMAT_R32G32B32_FLOAT,         0,      0,		D3D11_INPUT_PER_VERTEX_DATA,	0},
			{"COLOR",        0,      DXGI_FORMAT_R32G32B32A32_FLOAT,         0,     12,		D3D11_INPUT_PER_VERTEX_DATA,	0},
		};
	}VTXCOL;


	typedef struct ENGINE_DLL tagVertexPositionTexcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static constexpr string_view  Key = "VTXPOSTEX";
		static constexpr unsigned int					iElementCount = { 2 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[iElementCount] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
	}VTXPOSTEX;

	typedef struct ENGINE_DLL tagVertexPositionNormalTexcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		static constexpr string_view  Key = "VTXNORMTEX";
		static constexpr unsigned int					iElementCount = { 3 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[iElementCount] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
	}VTXNORMTEX;

	typedef struct ENGINE_DLL tagVertexMeshPosNorTexTan
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;
		XMFLOAT3		vBinormal;

		static constexpr string_view  Key = "VTXMESH";
		static constexpr unsigned int					iElementCount = { 5 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[iElementCount] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	}VTXMESH;

	typedef struct ENGINE_DLL tagVertexSkinnedMeshPosNorTexTan
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;
		XMFLOAT3		vBinormal;

		XMUINT4 vBlendIndex;
		XMFLOAT4 vBlendWeight;

		static constexpr string_view  Key = "VTXSKINMESH";
		static constexpr unsigned int					iElementCount = { 7 };
		static constexpr D3D11_INPUT_ELEMENT_DESC		Elements[iElementCount] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BLENDINDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 56, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 72, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
	}VTXSKINMESH;

	/* Particle */
	typedef struct ENGINE_DLL tagVertexInstancePoint
	{
		_float4 vRight{}, vUp{}, vLook{};
		_float4 vTraslate{};
		_float3 vVelocity{};
		_float2 vLifeTime{};
	}VTX_INSTANCE_POINT;

	typedef struct ENGINE_DLL tagVertexInstancePointElement
	{
		static constexpr string_view Key = "VTX_INSTANCE_POINT_ELEMENT";
		static constexpr unsigned int iElementCount = { 7 };
		static constexpr D3D11_INPUT_ELEMENT_DESC Elements[iElementCount] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},

			{"WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1},
			{"TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT,1,76, D3D11_INPUT_PER_INSTANCE_DATA, 1}
		};
	}VTX_INSTANCE_POINT_ELEMENT;
};