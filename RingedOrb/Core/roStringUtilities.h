#pragma once

#include <locale>
#include <codecvt>
#include <string>

static void TrimStringFromRight(std::string& s, int tl = 0) {
	s.erase(s.size() - tl, s.size());
}

static std::wstring ConvertToMultiByteString(std::string char8) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(char8);
}

static std::string ConvertToByteString(std::wstring char16) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.to_bytes(char16);
}

