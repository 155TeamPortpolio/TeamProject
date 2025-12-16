#pragma once

#include "Camera_Enum.h"
#include "Camera_Struct.h"

NS_BEGIN(Engine)

namespace CamUtil
{
	// "현재 시간(time)이 키프레임들 중 어느 구간(두 키 사이)에 속하는지"를 찾아서, 그 구간 안에서의 "보간 비율(0~1)"까지 같이 계산해주는 함수.
	ENGINE_DLL CamKeySegment FindKeySegment(const vector<CamKeyFrame>& keyframes, float time);

	// =========================================================================================================================
    ENGINE_DLL bool Save(const filesystem::path& path, const CamSequenceDesc& seq, string& outErrorMsg);
    ENGINE_DLL bool Load(const filesystem::path& path, CamSequenceDesc& outSeq, string& outErrorMsg);

    ENGINE_DLL ofstream OpenOut(const filesystem::path& filePath, bool truncate = true);
    ENGINE_DLL ifstream OpenIn(const filesystem::path& filePath);

	ENGINE_DLL void WriteHeader(ofstream& outFile, _uint magic, _uint version);
	ENGINE_DLL bool ReadHeader(ifstream& inFile, _uint expectedMagic, _uint& outVersion);

	template<typename T>
    ENGINE_DLL void WriteData(ofstream& outFile, const T& data);

	template<typename T>
    ENGINE_DLL bool ReadData(ifstream& inFile, T& outData);

    ENGINE_DLL void WriteBytes(ofstream& outFile, const void* data, size_t byteCount);
    ENGINE_DLL bool ReadBytes(ifstream& inFile, void* outData, size_t byteCount);

    ENGINE_DLL void WriteString(ofstream& outFile, const string& str);
    ENGINE_DLL bool ReadString(ifstream& inFile, string& outStr);

	template<typename T>
    ENGINE_DLL void WriteVector(ofstream& outFile, const vector<T>& values);

	template<typename T>
    ENGINE_DLL bool ReadVector(ifstream& inFile, vector<T>& outValues, _uint maxCount);

    ENGINE_DLL void WriteAlign(ofstream& outFile, _uint align);
    ENGINE_DLL bool ReadAlign(ifstream& inFile, _uint align);

    ENGINE_DLL bool AtomicReplaceFile(const filesystem::path& tempPath, const filesystem::path& targetPath, string& outErrorMsg);

    template<typename T>
    ENGINE_DLL inline void WriteData(ofstream& outFile, const T& data)
    {
        static_assert(is_trivially_copyable_v<T>, "T must be trivially copyable");
        assert(outFile.good());
        outFile.write(reinterpret_cast<const char*>(&data), sizeof(T));
    }

    template<typename T>
    ENGINE_DLL inline bool ReadData(ifstream& inFile, T& outData)
    {
        static_assert(is_trivially_copyable_v<T>, "T must be trivially copyable");
        inFile.read(reinterpret_cast<char*>(&outData), sizeof(T));
        return static_cast<bool>(inFile);
    }

    template<typename T>
    ENGINE_DLL inline void WriteVector(ofstream& outFile, const vector<T>& values)
    {
        static_assert(is_trivially_copyable_v<T>, "vector<T>: T must be trivially copyable");

        const size_t count64 = values.size();
        assert(count64 <= (numeric_limits<_uint>::max)());
        const _uint count = static_cast<_uint>(count64);

        WriteData(outFile, count);

        if (count == 0) return;

        assert(sizeof(T) <= (numeric_limits<size_t>::max)() / static_cast<size_t>(count));
        const size_t totalBytes = sizeof(T) * static_cast<size_t>(count);

        outFile.write(reinterpret_cast<const char*>(values.data()), static_cast<streamsize>(totalBytes));
    }

    template<typename T>
    ENGINE_DLL inline bool ReadVector(ifstream& inFile, vector<T>& outValues, _uint maxCount)
    {
        static_assert(is_trivially_copyable_v<T>, "vector<T>: T must be trivially copyable");

        _uint count = 0;
        if (!ReadData(inFile, count)) return false;
        if (count > maxCount) return false;

        if (count == 0)
        {
            outValues.clear();
            return true;
        }

        assert(sizeof(T) <= (numeric_limits<size_t>::max)() / static_cast<size_t>(count));
        const size_t totalBytes = sizeof(T) * static_cast<size_t>(count);

        outValues.resize(static_cast<size_t>(count));
        inFile.read(reinterpret_cast<char*>(outValues.data()), static_cast<streamsize>(totalBytes));
        return static_cast<bool>(inFile);
    }
}

NS_END