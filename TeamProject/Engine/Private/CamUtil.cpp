#include "Engine_Defines.h"
#include "CamUtil.h"
#include "Camera_Struct.h"

namespace
{
    enum class CamVersion : _uint
    {
        V1 = 1u,
    };

    constexpr _uint kCamMagic   = 0x43414D53u;
    constexpr _uint kCamVersion = static_cast<_uint>(CamVersion::V1);
}

CamKeySegment CamUtil::FindKeySegment(const vector<CamKeyFrame>& keyframes, float time)
{
	assert(keyframes.size() >= 2);

	const float firstTime = keyframes.front().time;
	const float lastTime  = keyframes.back().time;

	if (time <= firstTime)
		return { 0, 0.f };

	const _uint lastIdx = keyframes.size() - 1;
	if (time >= lastTime)
		return { lastIdx - 1, 1.f };

	_uint segmentIdx = 0;

	for (; segmentIdx + 1 < keyframes.size(); ++segmentIdx)
	{
		const float nextTime = keyframes[segmentIdx + 1].time;
		if (time < nextTime)
			break;
	}

	const float segmentStartTime = keyframes[segmentIdx].time;
	const float segmentEndTime   = keyframes[segmentIdx + 1].time;

	const float segmentDuration = segmentEndTime - segmentStartTime;
	assert(segmentDuration > 0.f);

	float normalizedTime = (time - segmentStartTime) / segmentDuration;
	normalizedTime = clamp(normalizedTime, 0.f, 1.f);

	return { segmentIdx, normalizedTime };
}
// =======================================================================================================
ofstream CamUtil::OpenOut(const filesystem::path& filePath, bool truncate)
{
	ios::openmode mode = ios::binary | ios::out;
	if (truncate) mode |= ios::trunc;
	ofstream outFile(filePath, mode);
	return outFile;
}

ifstream CamUtil::OpenIn(const filesystem::path& filePath)
{
	ifstream inFile(filePath, ios::binary | ios::in);
	return inFile;
}

void CamUtil::WriteHeader(ofstream& outFile, _uint magic, _uint version)
{
	assert(outFile.good());
	WriteData(outFile, magic);
	WriteData(outFile, version);
}

bool CamUtil::ReadHeader(ifstream& inFile, _uint expectedMagic, _uint& outVersion)
{
	_uint magic = 0;
	if (!ReadData(inFile, magic)) return false;
	if (magic != expectedMagic) return false;
	if (!ReadData(inFile, outVersion)) return false;
	return true;
}

void CamUtil::WriteBytes(ofstream& outFile, const void* data, size_t byteCount)
{
	assert(outFile.good());
	if (byteCount == 0) return;
	outFile.write(reinterpret_cast<const char*>(data), static_cast<streamsize>(byteCount));
}

bool CamUtil::ReadBytes(ifstream& inFile, void* outData, size_t byteCount)
{
	if (byteCount == 0) return true;
	inFile.read(reinterpret_cast<char*>(outData), static_cast<streamsize>(byteCount));
	return static_cast<bool>(inFile);
}

void CamUtil::WriteString(ofstream& outFile, const string& str)
{
	const size_t byteCount64 = str.size();
	assert(byteCount64 <= (numeric_limits<_uint>::max)());
	const _uint byteCount = static_cast<_uint>(byteCount64);

	WriteData(outFile, byteCount);
	if (byteCount == 0) return;

	outFile.write(str.data(), static_cast<streamsize>(byteCount));
}

bool CamUtil::ReadString(ifstream& inFile, string& outStr)
{
	_uint byteCount = 0;
	if (!ReadData(inFile, byteCount)) return false;

	if (byteCount == 0)
	{
		outStr.clear();
		return true;
	}

	const _uint kMaxStringBytes = 16u * 1024u * 1024u;
	if (byteCount > kMaxStringBytes) return false;

	outStr.resize(static_cast<size_t>(byteCount));
	inFile.read(outStr.data(), static_cast<streamsize>(byteCount));
	return static_cast<bool>(inFile);
}

void CamUtil::WriteAlign(ofstream& outFile, _uint align)
{
	assert(align > 0);

	const streamoff pos = outFile.tellp();
	const _uint padding = static_cast<_uint>((align - (pos % align)) % align);
	if (padding == 0) return;

	static const uint8_t zeros[16] = { 0 };
	_uint remain = padding;

	while (remain > 0)
	{
		const _uint chunk = (remain < 16u) ? remain : 16u;
		WriteBytes(outFile, zeros, static_cast<size_t>(chunk));
		remain -= chunk;
	}
}

bool CamUtil::ReadAlign(ifstream& inFile, _uint align)
{
	assert(align > 0);

	const streamoff pos = inFile.tellg();
	const _uint padding = static_cast<_uint>((align - (pos % align)) % align);
	if (padding == 0) return true;

	vector<uint8_t> scratch(static_cast<size_t>(padding));
	return ReadBytes(inFile, scratch.data(), scratch.size());
}

bool CamUtil::AtomicReplaceFile(const filesystem::path& tempPath, const filesystem::path& targetPath, string& outErrorMsg)
{
#ifdef _WIN32
	const wstring tempW = tempPath.wstring();
	const wstring targetW = targetPath.wstring();

	if (MoveFileExW(tempW.c_str(), targetW.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) == 0)
	{
		const DWORD e = GetLastError();
		outErrorMsg = "MoveFileExW failed: " + to_string(static_cast<unsigned long>(e));
		return false;
	}
	return true;
#else
	error_code ec;
	filesystem::rename(tempPath, targetPath, ec);
	if (!ec) return true;

	filesystem::remove(targetPath, ec);
	ec.clear();
	filesystem::rename(tempPath, targetPath, ec);
	if (ec)
	{
		outErrorMsg = "rename failed: " + ec.message();
		return false;
	}
	return true;
#endif
}

bool CamUtil::Save(const filesystem::path& path, const CamSequenceDesc& seq, string& outErrorMsg)
{
    outErrorMsg.clear();

    const filesystem::path tempPath = path.string() + ".tmp";

    ofstream outFile = CamUtil::OpenOut(tempPath, true);
    if (!outFile)
    {
        outErrorMsg = "Failed to open file: " + tempPath.string();
        return false;
    }

    WriteHeader(outFile, kCamMagic, kCamVersion);

    WriteString(outFile, seq.name);

    WriteData(outFile, static_cast<_uint>(seq.camType));
    WriteData(outFile, static_cast<_uint>(seq.rigType));
    WriteData(outFile, static_cast<_uint>(seq.projType));
    WriteData(outFile, static_cast<_uint>(seq.playbackMode));

    WriteData(outFile, static_cast<_uint>(seq.posInterp));
    WriteData(outFile, static_cast<_uint>(seq.rotInterp));
    WriteData(outFile, static_cast<_uint>(seq.fovInterp));

    const size_t keyCount64 = seq.keyframes.size();
    assert(keyCount64 <= (numeric_limits<_uint>::max)());
    const _uint keyCount = static_cast<_uint>(keyCount64);

    WriteData(outFile, keyCount);

    for (_uint i = 0; i < keyCount; ++i)
    {
        const CamKeyFrame& k = seq.keyframes[(size_t)i];

        WriteData(outFile, k.keyId);
        WriteData(outFile, k.time);

        WriteData(outFile, k.pos.x);
        WriteData(outFile, k.pos.y);
        WriteData(outFile, k.pos.z);

        WriteData(outFile, k.look.x);
        WriteData(outFile, k.look.y);
        WriteData(outFile, k.look.z);

        WriteData(outFile, k.roll);
        WriteData(outFile, k.fov);

        const uint8_t useCustom = k.useCustomInterp ? 1u : 0u;
        WriteData(outFile, useCustom);

        WriteData(outFile, static_cast<_uint>(k.outPosInterp));
        WriteData(outFile, static_cast<_uint>(k.outRotInterp));
        WriteData(outFile, static_cast<_uint>(k.outFovInterp));
    }

    outFile.flush();
    if (!outFile)
    {
        outErrorMsg = "Failed to write file: " + tempPath.string();
        outFile.close();
        return false;
    }

    outFile.close();

    if (!CamUtil::AtomicReplaceFile(tempPath, path, outErrorMsg))
    {
        error_code ec;
        filesystem::remove(tempPath, ec);
        return false;
    }

    return true;
}

bool CamUtil::Load(const filesystem::path& path, CamSequenceDesc& outSeq, string& outErrorMsg)
{
    outErrorMsg.clear();

    ifstream inFile = OpenIn(path);
    if (!inFile)
    {
        outErrorMsg = "Failed to open file: " + path.string();
        return false;
    }

    _uint version = 0;
    if (!ReadHeader(inFile, kCamMagic, version))
    {
        outErrorMsg = "Invalid header";
        return false;
    }

    if (version != kCamVersion)
    {
        outErrorMsg = "Unsupported version: " + to_string(static_cast<unsigned long>(version));
        return false;
    }

    if (!ReadString(inFile, outSeq.name))
    {
        outErrorMsg = "Failed to read name";
        return false;
    }

    _uint camType = 0;
    _uint rigType = 0;
    _uint projType = 0;
    _uint playbackMode = 0;
    _uint posInterp = 0;
    _uint rotInterp = 0;
    _uint fovInterp = 0;

    if (!ReadData(inFile, camType)) { outErrorMsg = "Read camType      failed"; return false; }
    if (!ReadData(inFile, rigType)) { outErrorMsg = "Read rigType      failed"; return false; }
    if (!ReadData(inFile, projType)) { outErrorMsg = "Read projType     failed"; return false; }
    if (!ReadData(inFile, playbackMode)) { outErrorMsg = "Read playbackMode failed"; return false; }
    if (!ReadData(inFile, posInterp)) { outErrorMsg = "Read posInterp    failed"; return false; }
    if (!ReadData(inFile, rotInterp)) { outErrorMsg = "Read rotInterp    failed"; return false; }
    if (!ReadData(inFile, fovInterp)) { outErrorMsg = "Read fovInterp    failed"; return false; }

    outSeq.camType = static_cast<CamType>(camType);
    outSeq.rigType = static_cast<CamRigType>(rigType);
    outSeq.projType = static_cast<CamProjType>(projType);
    outSeq.playbackMode = static_cast<CamPlaybackMode>(playbackMode);

    outSeq.posInterp = static_cast<CamPosInterp>(posInterp);
    outSeq.rotInterp = static_cast<CamRotInterp>(rotInterp);
    outSeq.fovInterp = static_cast<CamFovInterp>(fovInterp);

    _uint keyCount = 0;
    if (!ReadData(inFile, keyCount))
    {
        outErrorMsg = "Failed to read keyCount";
        return false;
    }

    const _uint kMaxKeys = 200000u;
    if (keyCount > kMaxKeys)
    {
        outErrorMsg = "Too many keys";
        return false;
    }

    outSeq.keyframes.clear();
    outSeq.keyframes.resize(static_cast<size_t>(keyCount));

    for (_uint i = 0; i < keyCount; ++i)
    {
        CamKeyFrame& k = outSeq.keyframes[static_cast<size_t>(i)];

        if (!ReadData(inFile, k.keyId)) { outErrorMsg = "Read keyId failed";   return false; }
        if (!ReadData(inFile, k.time)) { outErrorMsg = "Read time failed";    return false; }

        if (!ReadData(inFile, k.pos.x)) { outErrorMsg = "Read pos.x failed";   return false; }
        if (!ReadData(inFile, k.pos.y)) { outErrorMsg = "Read pos.y failed";   return false; }
        if (!ReadData(inFile, k.pos.z)) { outErrorMsg = "Read pos.z failed";   return false; }

        if (!ReadData(inFile, k.look.x)) { outErrorMsg = "Read look.x failed";  return false; }
        if (!ReadData(inFile, k.look.y)) { outErrorMsg = "Read look.y failed";  return false; }
        if (!ReadData(inFile, k.look.z)) { outErrorMsg = "Read look.z failed";  return false; }

        if (!ReadData(inFile, k.roll)) { outErrorMsg = "Read roll failed";    return false; }
        if (!ReadData(inFile, k.fov)) { outErrorMsg = "Read fov failed";     return false; }

        uint8_t useCustom = 0;
        _uint outPos = 0;
        _uint outRot = 0;
        _uint outFov = 0;

        if (!ReadData(inFile, useCustom)) { outErrorMsg = "Read useCustomInterp failed"; return false; }
        if (!ReadData(inFile, outPos)) { outErrorMsg = "Read outPosInterp failed";    return false; }
        if (!ReadData(inFile, outRot)) { outErrorMsg = "Read outRotInterp failed";    return false; }
        if (!ReadData(inFile, outFov)) { outErrorMsg = "Read outFovInterp failed";    return false; }

        k.useCustomInterp = (useCustom != 0);
        k.outPosInterp = static_cast<CamPosInterp>(outPos);
        k.outRotInterp = static_cast<CamRotInterp>(outRot);
        k.outFovInterp = static_cast<CamFovInterp>(outFov);
    }

    if (!inFile)
    {
        outErrorMsg = "File read error";
        return false;
    }

    return true;
}
