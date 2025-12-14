#ifndef Engine_Typedef_h__
#define Engine_Typedef_h__

namespace Engine
{
	typedef		bool						_bool;

	typedef		signed char					_byte;
	typedef		unsigned char				_ubyte;
	typedef		char						_char;	

	typedef		wchar_t						_tchar;
	typedef		wstring						_wstring;
	
	typedef		signed short				_short;
	typedef		unsigned short				_ushort;	

	typedef		signed int					_int;
	typedef		unsigned int				_uint;

	typedef		signed long					_long;
	typedef		unsigned long				_ulong;

	typedef		float						_float;
	typedef		double						_double;


	typedef		XMFLOAT2					_float2;
	typedef		XMFLOAT3					_float3;
	typedef		XMFLOAT4					_float4;
	typedef		XMFLOAT4X4			_float4x4;

	/* SIMD 연산을 위한 데이터 선언 */
	/* SIMD : 병렬적 연산, 안전한 메모리! 스택에 하는 것을 권장함 */
	typedef		XMVECTOR				_vector;
	typedef		FXMVECTOR				_fvector;
	typedef		CXMVECTOR				_cvector;
	typedef		GXMVECTOR				_gvector;
	typedef		HXMVECTOR				_hvector;

	typedef		XMMATRIX					_matrix;
	typedef		FXMMATRIX				_fmatrix;
	typedef		CXMMATRIX				_cmatrix;

	typedef		DirectX::SimpleMath::Vector2	_vector2;
	typedef		DirectX::SimpleMath::Vector3	_vector3;
	typedef		DirectX::SimpleMath::Vector4	_vector4;
	typedef		DirectX::SimpleMath::Matrix		_smatrix;
	typedef		DirectX::SimpleMath::Quaternion _quaternion;

	using LEVEL_CREATOR = function<class CLevel* ()>;
	static const string G_GlobalLevelKey = "Global_Level";
	static constexpr _uint g_iMaxNumBones = 16384;
	static constexpr _uint g_iMaxTransform = 4096;
	static constexpr _uint g_iMaxLightss = 46;
	static constexpr _uint MAX_TEXTURE_TYPE_VALUE = static_cast<_uint>(TEXTURE_TYPE::END);
	static constexpr _uint g_iMaxNumInstances = 3000;
	const unsigned int g_iMaxWidth = 8192;
	const unsigned int g_iMaxHeight = 4608;
}

#endif // Engine_Typedef_h__
