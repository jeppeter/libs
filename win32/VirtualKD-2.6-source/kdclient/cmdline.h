#pragma once

#include <bzscmn/bzsbuf.h>
#include <bzscmn/string.h>
#include <map>

struct RemoteProcessInfo
{
	BazisLib::DynamicStringW CommandLine;
	std::map<BazisLib::DynamicStringW, BazisLib::DynamicStringW> Environment;

	BazisLib::DynamicStringW GetEnvironmentVariable(const wchar_t *pwszVarName) const
	{
		std::map<BazisLib::DynamicStringW, BazisLib::DynamicStringW>::const_iterator it = Environment.find(pwszVarName);
		if (it == Environment.end())
			return L"";
		else
			return it->second;
	}
};

RemoteProcessInfo GetRemoteProcessInfo(DWORD PID);
RemoteProcessInfo GetLocalProcessInfo();