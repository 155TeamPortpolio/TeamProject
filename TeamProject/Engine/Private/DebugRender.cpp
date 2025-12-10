#include "DebugRender.h"
#include "Model.h"
#include "GameInstance.h"
CDebugRender::CDebugRender()
{
}

CDebugRender::CDebugRender(const CDebugRender& rhs)
	: CComponent(rhs)
{

}

HRESULT CDebugRender::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDebugRender::Initialize(COMPONENT_DESC* pArg)
{
	return S_OK;
}

HRESULT CDebugRender::Render_DebugBox(ID3D11DeviceContext* pContext, _uint DrawIndex)
{
	ID3D11Buffer* pVertexBuffers[] = { m_Vertices[DrawIndex]};
	_uint  pVertexStride[] = { sizeof(VTXPOS) };
	_uint  pVertexOffset[] = { 0 };

	pContext->IASetVertexBuffers(0, 1, pVertexBuffers, pVertexStride, pVertexOffset);
	pContext->IASetIndexBuffer(m_pIB, DXGI_FORMAT_R16_UINT, 0);
	pContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	pContext->DrawIndexed(24, 0, 0);

	return S_OK;
}

/*로컬 상의 인맥스를 넣어줘야 함*/
void CDebugRender::Add_DebugBounding(const MINMAX_BOX& box)
{
	_float3 vMin = box.vMin;
	_float3 vMax = box.vMax;

	vMin.x -= 0.01f;	vMax.x += 0.01f;
	vMin.y -= 0.01f;	vMax.y += 0.01f;
	vMin.z -= 0.01f;	vMax.z += 0.01f;

	vector<VTXPOS> VB;
	VB.resize(8);

	VB[0].vPosition = { vMin.x, vMin.y,   vMin.z };
	VB[1].vPosition = { vMax.x,  vMin.y, vMin.z };
	VB[2].vPosition = { vMax.x, vMax.y, vMin.z };
	VB[3].vPosition = { vMin.x, vMax.y,  vMin.z };

	VB[4].vPosition = { vMin.x,  vMin.y,  vMax.z };
	VB[5].vPosition = { vMax.x, vMin.y,  vMax.z };
	VB[6].vPosition = { vMax.x, vMax.y, vMax.z };
	VB[7].vPosition = { vMin.x,  vMax.y, vMax.z };

	D3D11_BUFFER_DESC VBDesc;
	VBDesc.ByteWidth = sizeof(VTXPOS) * 8;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = sizeof(VTXPOS);

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = VB.data();

	ID3D11Buffer* pVB;
	HRESULT hr = CGameInstance::GetInstance()->Get_Device()->CreateBuffer(&VBDesc, &subData, &pVB);

	if (nullptr == m_pIB) {
		D3D11_BUFFER_DESC IDDesc;
		IDDesc.ByteWidth = sizeof(_ushort) * 24;
		IDDesc.Usage = D3D11_USAGE_DEFAULT;
		IDDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		IDDesc.CPUAccessFlags = 0;
		IDDesc.MiscFlags = 0;
		IDDesc.StructureByteStride = sizeof(_ushort);
		vector<_ushort> IB;
		IB.resize(24, 0);
		int i = 0;

		IB[i++] = 0;	IB[i++] = 1;		IB[i++] = 1;	IB[i++] = 2;		IB[i++] = 2;	IB[i++] = 3;		IB[i++] = 3;	IB[i++] = 0;
		IB[i++] = 4;	IB[i++] = 5;		IB[i++] = 5;	IB[i++] = 6;		IB[i++] = 6;	IB[i++] = 7;		IB[i++] = 7;	IB[i++] = 4;
		IB[i++] = 0;	IB[i++] = 4;		IB[i++] = 1;	IB[i++] = 5;		IB[i++] = 2;	IB[i++] = 6;		IB[i++] = 3;	IB[i++] = 7;

		D3D11_SUBRESOURCE_DATA subIndexData = {};
		subIndexData.pSysMem = IB.data();
		hr = CGameInstance::GetInstance()->Get_Device()->CreateBuffer(&IDDesc, &subIndexData, &m_pIB);
	}
	
	if (FAILED(hr)) {
		Safe_Release(pVB);
		Safe_Release(m_pIB);
	}

	m_Vertices.push_back(pVB);
}

CDebugRender* CDebugRender::Create()
{
	CDebugRender* instance = new CDebugRender();
	if (FAILED(instance->Initialize_Prototype()))
	{
		Safe_Release(instance);
		MSG_BOX("CDebugRender Comp Failed To Create : CDebugRender");
	}
	return instance;
}

CComponent* CDebugRender::Clone()
{
	return new CDebugRender(*this);
}

void CDebugRender::Free()
{
	__super::Free();
	for (auto& vertex : m_Vertices) {
		Safe_Release(vertex);
	}
	Safe_Release(m_pIB);
}
