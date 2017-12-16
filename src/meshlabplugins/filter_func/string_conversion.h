#ifndef __STRING_CONVERSION_H__
#define __STRING_CONVERSION_H__

#include<string>

namespace conversion
{
	static std::wstring fromStringToWString(const std::string& input)
	{
		std::wstring tmp;
		tmp.assign(input.begin(), input.end());
		return tmp;
	}

	static std::string fromWStringToString(const std::wstring& input)
	{
		std::string tmp;
		tmp.assign(input.begin(), input.end());
		return tmp;
	}
}

#endif