#include "Engine_Defines.h"
#include "Transform.h"

CTransform::CTransform()
{
}

CTransform::CTransform(const CTransform& rhs)
	:CComponent(rhs), m_LocalMatrix{ rhs.m_LocalMatrix },
	m_WorldMatrix{ rhs.m_WorldMatrix },
	m_WorldInversMatrix{ rhs.m_WorldInversMatrix }
{
}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_LocalMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_WorldInversMatrix, XMMatrixIdentity());
	return S_OK;
}

HRESULT CTransform::Initialize(COMPONENT_DESC* pArg)
{
	_vector  quaternion = XMQuaternionIdentity();
	XMStoreFloat4(&m_qRotation, quaternion);

	if (pArg == nullptr)
		return S_OK;

	TRANSFORM_DESC* desc = static_cast<TRANSFORM_DESC*>(pArg);

	_fvector position = XMVectorSetW(XMLoadFloat3(&desc->vInitialPosition), 1.f);
	XMStoreFloat4(&m_vPosition, position);
	_fvector scale = XMVectorSetW(XMLoadFloat3(&desc->vInitialScale), 0.f);
	XMStoreFloat4(&m_vScale, scale);

	/*오일러로 받아서 -> 쿼터니언으로*/
	_vector euler = XMVectorSetW(XMLoadFloat3(&desc->vInitialEulerVector), 0.f);
	  quaternion = XMQuaternionRotationRollPitchYawFromVector(euler);

	if (XMVector4Equal(quaternion, XMVectorZero()))
		quaternion = XMQuaternionIdentity();

	quaternion = XMQuaternionNormalize(quaternion); // 항상 정규화
	XMStoreFloat4(&m_qRotation, quaternion);

	MarkDirty();

	return S_OK;
}

void CTransform::Translate(_fvector momentVector)
{
	_vector vPos = XMLoadFloat4(&m_vPosition);
	vPos += momentVector;

	XMStoreFloat4(&m_vPosition, vPos);
	MarkDirty();

}


/*누적 회전*/
void CTransform::Rotation(_fvector eulerVector)//로테이션으로 누적해서 돌리기
{
	_fvector addQuaternion = XMQuaternionRotationRollPitchYawFromVector(eulerVector);
	_fvector myQuaternion = XMLoadFloat4(&m_qRotation);
	_fvector newQuaternion = XMQuaternionMultiply(myQuaternion, addQuaternion);
	_vector finalQuaternion = XMQuaternionNormalize(newQuaternion);
	XMStoreFloat4(&m_qRotation, finalQuaternion);
	MarkDirty();

}

/*누적 회전*/
void CTransform::Rotation(_fvector vAxis, _float fRadian) //라디안으로 축회전하기
{
	_fvector addQuaternion = XMQuaternionRotationAxis(vAxis, fRadian);
	_fvector myQuaternion = XMLoadFloat4(&m_qRotation);
	_fvector newQuaternion = XMQuaternionMultiply(addQuaternion, myQuaternion);
	_vector finalQuaternion = XMQuaternionNormalize(newQuaternion);
	XMStoreFloat4(&m_qRotation, finalQuaternion);
	MarkDirty();

}


void CTransform::AddScale(_fvector scale)
{
	_vector nowScale = XMLoadFloat4(&m_vScale);
	nowScale += scale;

	XMStoreFloat4(&m_vScale, nowScale);
	MarkDirty();

}

void CTransform::Set_Pos(const _float3& position)
{
	if (XMVector3NotEqual(XMLoadFloat4(&m_vPosition), XMVectorSet(position.x, position.y, position.z, 1.f)))
	{
		_fvector newPosition = XMVectorSetW(XMLoadFloat3(&position), 1.f);
		XMStoreFloat4(&m_vPosition, newPosition);
		MarkDirty();
	}
}

void CTransform::Set_Pos(const _float4& position)
{
	if (XMVector3NotEqual(XMLoadFloat4(&m_vPosition), XMVectorSet(position.x, position.y, position.z, 1.f)))
	{
		_fvector newPosition = XMVectorSetW(XMLoadFloat4(&position), 1.f);
		XMStoreFloat4(&m_vPosition, newPosition);
		MarkDirty();
	}
}

void CTransform::Set_PosVector(_fvector position)
{
	if (XMVector3NotEqual(XMLoadFloat4(&m_vPosition), position))
	{
		XMStoreFloat4(&m_vPosition, XMVectorSetW(position,1.f));
		MarkDirty();
	}
}

void CTransform::Set_vectorPos(_fvector position)
{
	if (XMVector3NotEqual(XMLoadFloat4(&m_vPosition), position))
	{
		_fvector newPosition = XMVectorSetW(position, 1.f);
		XMStoreFloat4(&m_vPosition, newPosition);
		MarkDirty();
	}
}

void CTransform::Set_Y(const _float& position)
{
	m_vPosition.y = position;
	MarkDirty();
}

void CTransform::Rotate(const _float3& _eular)
{
	_fvector euler = XMVectorSetW(XMLoadFloat3(&_eular), 0.f);
	_fvector  quaternion = XMQuaternionRotationRollPitchYawFromVector(euler);
	XMStoreFloat4(&m_qRotation, XMQuaternionNormalize(quaternion));
	MarkDirty();

}

void CTransform::Scale(const _float3& scale)
{
	_fvector newScale = XMVectorSetW(XMLoadFloat3(&scale), 0.f);
	XMStoreFloat4(&m_vScale, newScale);
	MarkDirty();

}

void CTransform::Scale_Vector(_fvector scale)
{
	XMStoreFloat4(&m_vScale, scale);
	MarkDirty();
}

_float4x4* CTransform::Get_WorldMatrix_Ptr()
{
	if (Check_Dirty())
		Update_Transform();

	return &m_WorldMatrix;
}

_float4x4 CTransform::Get_WorldMatrix()
{
	if (Check_Dirty())
		Update_Transform();

	return m_WorldMatrix;
}

_float4x4* CTransform::Get_LocalMatrix()
{
	if (Check_Dirty())
		Update_Transform();

	return &m_LocalMatrix;
}

_float4x4 CTransform::Get_InverseWorldMatrix()
{
	if (Check_Dirty())
		Update_Transform();

	return m_WorldInversMatrix;
}

_float4x4* CTransform::Get_InverseWorldMatrix_Ptr()
{
	if (Check_Dirty())
		Update_Transform();

	return &m_WorldInversMatrix;
}

_vector CTransform::Get_WorldPos()
{
	if (m_pParentTransform) {
		_vector worldpos = XMVector3Transform(XMLoadFloat4(&m_vPosition), XMLoadFloat4x4(m_pParentTransform->Get_WorldMatrix_Ptr()));
		return worldpos;
	}
	else
		return Get_Pos();
}

/*노말라이즈 되어서 반환*/
_vector CTransform::Dir(STATE eState)
{
	if (Check_Dirty())
		Update_Transform();

	_matrix worldMat = XMLoadFloat4x4(&m_WorldMatrix);
	return XMVector3Normalize(worldMat.r[static_cast<int>(eState)]);
}

void CTransform::Set_ParentTransform(CTransform* pParentTransform)
{
	m_pParentTransform = pParentTransform;
	MarkDirty();

}

void CTransform::TranslateMatrix(_fmatrix matrix)
{
	_vector vScale;
	_vector qRotation;
	_vector vTrans;
	if (XMMatrixDecompose(&vScale, &qRotation, &vTrans, matrix))
	{
		XMStoreFloat4(&m_vScale, vScale);
		XMStoreFloat4(&m_qRotation, qRotation);
		XMStoreFloat4(&m_vPosition, vTrans);
	}

	MarkDirty();

}

void CTransform::Render_GUI()
{
	ImGui::SeparatorText("Transform");
	float childWidth = ImGui::GetContentRegionAvail().x;
	const float textLineHeight = ImGui::GetTextLineHeightWithSpacing();
	const float childHeight = (textLineHeight * 8) + (ImGui::GetStyle().WindowPadding.y * 2);

	ImGui::BeginChild("##TransformChild", ImVec2{ 0, childHeight }, true);
	ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 1.f), "Position");
	ImGui::InputFloat3("##Position", reinterpret_cast<float*>(&m_vPosition), "%.1f", ImGuiInputTextFlags_ReadOnly);
	ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 1.f), "Rotation");
	ImGui::InputFloat4("##Rotation", reinterpret_cast<float*>(&m_qRotation), "%.4f", ImGuiInputTextFlags_ReadOnly);
	ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 1.f), "Scale");
	ImGui::InputFloat3("##Scale", reinterpret_cast<float*>(&m_vScale), "%.1f", ImGuiInputTextFlags_ReadOnly);
	ImGui::EndChild();
}

void CTransform::LookAt(_fvector vAt)
{
	//목표하는 방향 벡터. 목표 - 현재
	_fvector vLookDir = XMVector3Normalize(vAt - XMLoadFloat4(&m_vPosition));

	/*
		직교 좌표계를 다시 구성하고, 거기에 내 포지션을 넣어서 회전 행렬을 만들어줄 거임.
	*/

	//내 위치 벡터
	_vector vPos = XMLoadFloat4(&m_vPosition);
	//월드업 벡터
	_fvector vWorldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	//RightVecto 구하기 - 라업룩  : 업->룩
	_fvector vRight = XMVector3Normalize(XMVector3Cross(vWorldUp, vLookDir));
	//UpVector 구하기 - 라업룩  : 룩->라
	_fvector vUp = XMVector3Cross(vLookDir, vRight);

	_matrix vRotmat = XMMatrixIdentity();
	vRotmat.r[0] = vRight;
	vRotmat.r[1] = vUp;
	vRotmat.r[2] = vLookDir;

	_vector vQuaternion = XMQuaternionRotationMatrix(vRotmat);
	XMStoreFloat4(&m_qRotation, vQuaternion);

	MarkDirty();

}

void CTransform::Override_Rotation(_fvector vAxis, _float fRadian)
{
	_fvector newQuaternion = XMQuaternionRotationAxis(vAxis, fRadian);
	_vector finalQuaternion = XMQuaternionNormalize(newQuaternion);
	XMStoreFloat4(&m_qRotation, finalQuaternion);
	MarkDirty();

}

void CTransform::Reset_Rotation()
{
	m_qRotation = { 0.f,0.f,0.f,1.f };
	MarkDirty();
}

void CTransform::Update_Transform()
{
	if (m_pParentTransform && m_pParentTransform->m_bDirty)
		m_pParentTransform->Update_Transform();

	_matrix matScale = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z);

	_vector vQuaternion = XMLoadFloat4(&m_qRotation);
	_matrix matRot = XMMatrixRotationQuaternion(vQuaternion);

	_matrix matPos = XMMatrixTranslation(m_vPosition.x, m_vPosition.y, m_vPosition.z);

	_matrix LocalMatrix = matScale * matRot * matPos;

	XMStoreFloat4x4(&m_LocalMatrix, LocalMatrix);

	_matrix combined;

	if (m_pParentTransform) {
		combined = XMLoadFloat4x4(&m_LocalMatrix) * XMLoadFloat4x4(m_pParentTransform->Get_WorldMatrix_Ptr());
		XMStoreFloat4x4(&m_WorldMatrix, combined);
	}
	else {
		combined = XMLoadFloat4x4(&m_LocalMatrix);
		XMStoreFloat4x4(&m_WorldMatrix, combined);
	}

	XMStoreFloat4x4(&m_WorldInversMatrix, XMMatrixInverse(nullptr, combined));
	m_bDirty = false;
	++m_VersionCounter;
	if (m_pParentTransform)
		m_ParentVersionCounter = m_pParentTransform->m_VersionCounter;
}

_bool CTransform::Check_Dirty()
{
	bool dirty = m_bDirty;

	if (m_pParentTransform)
	{
		// 부모가 실제 Dirty였는지 확인 (조상 포함)
		if (m_pParentTransform->Check_Dirty())
			dirty = true;

		// 부모 버전 변화 체크
		if (m_ParentVersionCounter != m_pParentTransform->m_VersionCounter)
		{
			dirty = true;
			m_ParentVersionCounter = m_pParentTransform->m_VersionCounter;
		}
	}

	return dirty;
}


void CTransform::MarkDirty()
{
	m_bDirty = true;
	m_VersionCounter++;
}



CTransform* CTransform::Create()
{
	CTransform* instance = new CTransform();

	if (FAILED(instance->Initialize_Prototype())) {
		MSG_BOX("Transform Create Failed : CTransform");
		Safe_Release(instance);
	}

	return instance;
}

CComponent* CTransform::Clone()
{
	CTransform* instance = new CTransform(*this);
	return instance;
}

void CTransform::Free()
{
	__super::Free();
	Safe_Release(m_pParentTransform);
}

