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

	/*해당 디렉토리에 있는지*/
	ENGINE_DLL _bool IsUnderDirectory(const filesystem::path& file, const filesystem::path& dir);
	
};

namespace Math
{
	//Float 보간 함수
	ENGINE_DLL _float Lerp(_float x, _float y, _float t);

	ENGINE_DLL _float EaseOutCubic(_float t);   // EaseOutCubic:   빠르게 출발하고 끝에서 부드럽게 감속(카메라 이동 기본값으로 무난)
	ENGINE_DLL _float EaseInOutSine(_float t);  // EaseInOutSine:  시작/끝이 가장 자연스럽게 부드러운 S-curve(안정적인 일반 카메라)
	ENGINE_DLL _float EaseInOutCubic(_float t); // EaseInOutCubic: InOutSine보다 가속/감속이 더 뚜렷한 S-curve(연출/전투에 힘 있는 느낌)
	ENGINE_DLL _float EaseInCubic(_float t);    // EaseInCubic:    초반 움직임을 숨기고 뒤쪽에서 확 가속(컷 시작에서 튐 줄이기)
	ENGINE_DLL _float EaseOutSine(_float t);    // EaseOutSine:    감속이 가볍고 깔끔한 마무리(짧은 이동에 부담 없음)
	ENGINE_DLL _float EaseInQuad(_float t);     // EaseInQuad:     InCubic보다 약한 초반 가속(작은 이동에 살짝 뜸 들이기)
	ENGINE_DLL _float EaseOutQuad(_float t);    // EaseOutQuad:    OutCubic보다 단순한 감속(짧은 전환/미세 조정에 깔끔)
	ENGINE_DLL _float EaseInOutQuad(_float t);  // EaseInOutQuad:  직선적인 느낌의 명확한 S-curve(툴/편집기에서 보기 좋은 반응)
	ENGINE_DLL _float EaseInOutExpo(_float t);  // EaseInOutExpo:  초반 거의 정지→중간 매우 빠름→끝 부드러움(거리 큰 연출용, 일상엔 과함)
	ENGINE_DLL _float EaseOutBack(_float t);    // EaseOutBack:    목표를 살짝 지나쳤다 되돌아오는 오버슛(줌/락온 강조 연출, 과하면 멀미)
}