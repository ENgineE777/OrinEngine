#pragma once

#include <string>

namespace Oak::StringUtils
{
	void Init();
	int  GetLen(const char* str);
	bool IsEmpty(const char* str);
	const char* StrStr(const char* str, const char* sub_str);
	bool IsEqual(const char* str1, const char* str2);
	void Copy(char * str1, int len, const char* str2);
	void Cat(char* str1, int len, const char* str2);
	void Printf(char* str, int len, const char* format, ...);
	const char* PrintTemp(const char* format, ...);
	void RemoveSlashes(char* fullPath);
	void FixSlashes(char * str);
	void RemoveFirstChar(char* str);
	void GetFileName(const char* fullPath, char* fileName);
	bool GetPath(const char* fullPath, char* path);
	void GetCropPath(const char* relativePath, const char* fullPath, char* path, int len);
	void GetRootPath(const char* path1, const char* path2, char* root);
	void GetExtension(const char* str, char* ext, int ext_lenght);
	void RemoveExtension(char* str);
	void Replace(std::string& str, const char* from, const char* to);
	int  GetNameNumber(const char* str, char* wo_n_str, int len);
	void EscapeChars(const char* in, char* out, int len);
	bool BuildUtf16fromUtf8(char c, int& bytes, int& w);
	void BuildUtf8fromUtf16(int c, std::string& dest);
	void Utf8toUtf16(std::wstring& dest, const char* src);
	void Utf16toUtf8(std::string& dest, const wchar_t* src);
	void LowerCase(std::string& str);
	void UpperCase(std::string& str);
	bool CompareABC(const char* str1, const char* str2);
}