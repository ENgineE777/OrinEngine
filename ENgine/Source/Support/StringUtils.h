#pragma once

#include <eastl/string.h>
#include "Defines.h"

/**
\ingroup gr_code_common
*/

namespace Oak::StringUtils
{
	CLASS_DECLSPEC void Init();
	CLASS_DECLSPEC int  GetLen(const char* str);
	CLASS_DECLSPEC bool IsEmpty(const char* str);
	CLASS_DECLSPEC const char* StrStr(const char* str, const char* sub_str);
	CLASS_DECLSPEC bool IsEqual(const char* str1, const char* str2);
	CLASS_DECLSPEC void Copy(char * str1, int len, const char* str2);
	CLASS_DECLSPEC void Cat(char* str1, int len, const char* str2);
	CLASS_DECLSPEC void Printf(char* str, int len, const char* format, ...);
	CLASS_DECLSPEC const char* PrintTemp(const char* format, ...);
	CLASS_DECLSPEC void RemoveSlashes(char* fullPath);
	CLASS_DECLSPEC void FixSlashes(char * str);
	CLASS_DECLSPEC void RemoveFirstChar(char* str);
	CLASS_DECLSPEC void GetFileName(const char* fullPath, char* fileName);
	CLASS_DECLSPEC bool GetPath(const char* fullPath, char* path);
	CLASS_DECLSPEC void GetCropPath(const char* relativePath, const char* fullPath, char* path, int len);
	CLASS_DECLSPEC void GetRootPath(const char* path1, const char* path2, char* root);
	CLASS_DECLSPEC void GetExtension(const char* str, char* ext, int ext_lenght);
	CLASS_DECLSPEC void RemoveExtension(char* str);
	CLASS_DECLSPEC void Replace(eastl::string& str, const char* from, const char* to);
	CLASS_DECLSPEC void InreaseNumberInName(char* str, int len);
	CLASS_DECLSPEC void EscapeChars(const char* in, char* out, int len);
	CLASS_DECLSPEC bool BuildUtf16fromUtf8(char c, int& bytes, int& w);
	CLASS_DECLSPEC void BuildUtf8fromUtf16(int c, eastl::string& dest);
	CLASS_DECLSPEC void Utf8toUtf16(eastl::wstring& dest, const char* src);
	CLASS_DECLSPEC void Utf16toUtf8(eastl::string& dest, const wchar_t* src);
	CLASS_DECLSPEC void LowerCase(eastl::string& str);
	CLASS_DECLSPEC void UpperCase(eastl::string& str);
	CLASS_DECLSPEC bool CompareABC(const char* str1, const char* str2);
	CLASS_DECLSPEC void FindAndReplace(eastl::string& str, eastl::string from, eastl::string to);
}