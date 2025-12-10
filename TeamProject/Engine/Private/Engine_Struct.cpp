#include "Engine_Defines.h"
namespace Engine {
	//typedef struct D3D11_INPUT_ELEMENT_DESC
	//{
	//    LPCSTR SemanticName; -> 셰이더 매핑될 이름
	//    UINT SemanticIndex;  -> 셰이더에 매핑될 인덱스 (텍스트 0 , 텍스트 1 같은 것인듯)
	//    DXGI_FORMAT Format; -> 사용할 바이트 포맷
	//    UINT InputSlot; -> 장착할 슬롯
	//    UINT AlignedByteOffset; 지금 이게 시작할 메모리 바이트오프셋/ 
	//    D3D11_INPUT_CLASSIFICATION InputSlotClass = 사용 용도;
	//    UINT InstanceDataStepRate = 인스턴싱용;
	//} 	D3D11_INPUT_ELEMENT_DESC = 인스턴싱용;

	const D3D11_INPUT_ELEMENT_DESC		VTXPOS::Elements[] = {
		{"POSITION",        0,      DXGI_FORMAT_R32G32B32A32_FLOAT,         0,      0,		D3D11_INPUT_PER_VERTEX_DATA,	0},
	};

	const D3D11_INPUT_ELEMENT_DESC VTXPOSTEX::Elements[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

};