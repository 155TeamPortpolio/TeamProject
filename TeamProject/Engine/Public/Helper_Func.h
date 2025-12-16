#pragma once
#include "Engine_Defines.h"
#include <random>

#include "Engine_Math.h"

namespace Helper
{
	//�ƽ�Ű �ڵ尡 �ƴ� ���� ���ԵǾ� �ִ��� Ȯ��
	ENGINE_DLL _bool ContainsNonAscii(const string& str);
	//�ַ�� ���� �ȿ� �ִ� ������ Ȯ��
	ENGINE_DLL _bool IsPathInProjectFolder(const string& path);
	//���� ���ÿ� Ž����
	ENGINE_DLL string OpenFile_Dialogue();
	//���� ���ÿ� Ž����
	ENGINE_DLL string OpenFolder_Dialogue();
	//���� ���� ��������
	ENGINE_DLL vector<string> OpenMultiFolders();
	//���� ���� ��������
	ENGINE_DLL vector<string> OpenMultiFiles();
	//���� ����� Ž����
	ENGINE_DLL string SaveFileDialog();
	//���� ����� Ž���� - nfd���
	ENGINE_DLL string SaveFileDialog(const string& fileName, const string& filter);
	//���� ����� Ž���� - �⺻ ���� ����, �̸� ���� ����
	ENGINE_DLL string SaveFileDialogByWinAPI(const string& fileName, const string& filterStr);
	//wstring���� ��ȯ
	ENGINE_DLL wstring ConvertToWideString(const string& str);
	//string���� ��ȯ
	ENGINE_DLL string ConvertToString(const wstring& wstr);
	//���ϸ�+Ȯ���� ����
	ENGINE_DLL string GetFileNameWithExtension(const string& filePath);
	//Ȯ���ڸ� ����, ���ϸ� ����
	ENGINE_DLL string GetFileNameWithOutExtension(const string& filePath);
	//��� �ҹ��ڷ� ��ȯ
	ENGINE_DLL string  ToLower(const string& fileName);
	//��ο� �ؽ�ó ����
	ENGINE_DLL HRESULT SaveTextureToDDs(ID3D11DeviceContext* pContext, const string& filePath, ID3D11ShaderResourceView* pSRV);
	ENGINE_DLL HRESULT SaveTextureToDDs(ID3D11DeviceContext* pContext, const wstring& filePath, ID3D11ShaderResourceView* pSRV);
	//���� �Լ�
	ENGINE_DLL _int Get_Random_Int(_int min, _int max);
	ENGINE_DLL _float Get_Random_Float(_float min, _float max);

	/*�ش� ���丮�� �ִ���*/
	ENGINE_DLL _bool IsUnderDirectory(const filesystem::path& file, const filesystem::path& dir);

}

//json�� �ҷ� ���ϴ� ����ü�� ��ȯ
namespace Helper
{
	template <typename T>
	inline T GetDataFromJson(const string& filePath) {
		std::ifstream file(filePath);
		if (!file.is_open())
			return T{};

		json j;
		file >> j;
		return j.get<T>();
	};

	template <typename T>
	inline void SaveJson(T& Data, const string& filePath) {
		//json JsonData = Data;
		nlohmann::ordered_json JsonData = Data;
		ofstream file(filePath);

		if (file.is_open()) {
			file << JsonData.dump(2);
			file.close();
		}
	};
}

namespace Math
{
	//Float ���� �Լ�
	ENGINE_DLL _float Lerp(_float x, _float y, _float t);

	ENGINE_DLL _float EaseOutCubic(_float t);   // EaseOutCubic:   ������ ����ϰ� ������ �ε巴�� ����(ī�޶� �̵� �⺻������ ����)
	ENGINE_DLL _float EaseInOutSine(_float t);  // EaseInOutSine:  ����/���� ���� �ڿ������� �ε巯�� S-curve(�������� �Ϲ� ī�޶�)
	ENGINE_DLL _float EaseInOutCubic(_float t); // EaseInOutCubic: InOutSine���� ����/������ �� �ѷ��� S-curve(����/������ �� �ִ� ����)
	ENGINE_DLL _float EaseInCubic(_float t);    // EaseInCubic:    �ʹ� �������� ����� ���ʿ��� Ȯ ����(�� ���ۿ��� Ʀ ���̱�)
	ENGINE_DLL _float EaseOutSine(_float t);    // EaseOutSine:    ������ ������ ����� ������(ª�� �̵��� �δ� ����)
	ENGINE_DLL _float EaseInQuad(_float t);     // EaseInQuad:     InCubic���� ���� �ʹ� ����(���� �̵��� ��¦ �� ���̱�)
	ENGINE_DLL _float EaseOutQuad(_float t);    // EaseOutQuad:    OutCubic���� �ܼ��� ����(ª�� ��ȯ/�̼� ������ ���)
	ENGINE_DLL _float EaseInOutQuad(_float t);  // EaseInOutQuad:  �������� ������ ��Ȯ�� S-curve(��/�����⿡�� ���� ���� ����)
	ENGINE_DLL _float EaseInOutExpo(_float t);  // EaseInOutExpo:  �ʹ� ���� �������߰� �ſ� �����泡 �ε巯��(�Ÿ� ū �����, �ϻ� ����)
	ENGINE_DLL _float EaseOutBack(_float t);    // EaseOutBack:    ��ǥ�� ��¦ �����ƴ� �ǵ��ƿ��� ������(��/���� ���� ����, ���ϸ� �ֹ�)
}

