#pragma once
#include "Engine_Defines.h"
#include <random>

namespace Helper
{
	//아스키 코드가 아닌 것이 포함되어 있는지 확인
	ENGINE_DLL _bool ContainsNonAscii(const string& str);
	//솔루션 폴더 안에 있는 것인지 확인
	ENGINE_DLL _bool IsPathInProjectFolder(const string& path);
	//파일 선택용 탐색기
	ENGINE_DLL string OpenFile_Dialogue();
	//폴더 선택용 탐색기
	ENGINE_DLL string OpenFolder_Dialogue();
	//여러 폴더 가져오기
	ENGINE_DLL vector<string> OpenMultiFolders();
	//여러 파일 가져오기
	ENGINE_DLL vector<string> OpenMultiFiles();
	//파일 저장용 탐색기
	ENGINE_DLL string SaveFileDialog();
	//파일 저장용 탐색기 - nfd기반
	ENGINE_DLL string SaveFileDialog(const string& fileName, const string& filter);
	//파일 저장용 탐색기 - 기본 파일 형식, 이름 지정 가능
	ENGINE_DLL string SaveFileDialogByWinAPI(const string& fileName, const string& filterStr);
	//wstring으로 변환
	ENGINE_DLL wstring ConvertToWideString(const string& str);
	//string으로 변환
	ENGINE_DLL string ConvertToString(const wstring& wstr);
	//파일명+확장자 추출
	ENGINE_DLL string GetFileNameWithExtension(const string& filePath);
	//확장자명 제외, 파일명 추출
	ENGINE_DLL string GetFileNameWithOutExtension(const string& filePath);
	//모두 소문자로 변환
	ENGINE_DLL string  ToLower(const string& fileName);
	//경로에 텍스처 저장
	ENGINE_DLL HRESULT SaveTextureToDDs(ID3D11DeviceContext* pContext,const string& filePath, ID3D11ShaderResourceView* pSRV);
	ENGINE_DLL HRESULT SaveTextureToDDs(ID3D11DeviceContext* pContext,const wstring& filePath, ID3D11ShaderResourceView* pSRV);
	//랜덤 함수
	ENGINE_DLL _int Get_Random_Int(_int min, _int max);
	ENGINE_DLL _float Get_Random_Float(_float min, _float max);
};

