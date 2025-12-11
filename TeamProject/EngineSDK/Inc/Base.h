#pragma once

NS_BEGIN(Engine)

class  ENGINE_DLL CBase abstract
{
protected:
	explicit CBase() =default;
	virtual ~CBase() = default;

	public:
		virtual _uint AddRef() ;
		virtual _uint Release();

protected:
	unsigned int			m_iRefCnt = 0;

public:
	virtual void Free();

};

NS_END