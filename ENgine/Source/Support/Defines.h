
#pragma once

#ifdef OAK_EXPORTING
#define CLASS_DECLSPEC __declspec(dllexport)
#else
#define CLASS_DECLSPEC __declspec(dllimport)
#endif

#define RELEASE(p) if (p) { p->Release(); p = nullptr; }
#define FREE_PTR(p) if (p) { free(p); p = nullptr; }
#define DELETE_PTR(p) if (p) { delete p; p = nullptr; }
#define DELETE_ARRAY(p) if (p) { delete[] p; p = nullptr; }

void* operator new(size_t size, const char* file, int line);
void* operator new[](size_t size, const char* file, int line);

void operator delete(void* ptr, const char* file, int line);
void operator delete[](void* ptr, const char* file, int line);

#define NEW new(__FILE__, __LINE__)

#ifdef OAK_EDITOR

#define OAK_ASSERT(expression, description) \
	if (!(expression))\
	{\
		MessageBoxA(nullptr, description, "Assert", MB_ICONERROR);\
	}

#define OAK_ALERT(description) \
	{\
		root.Log("Alert", description);\
		MessageBoxA(nullptr, description, "Alert", MB_ICONERROR);\
	}

#define MESSAGE_BOX(caption, text) MessageBoxA(nullptr, text, caption, MB_ICONERROR);

#else

#define OAK_ASSERT(expression, description)
#define OAK_ALERT(description)
#define MESSAGE_BOX(caption, text)

#endif

#define _FL_ __FILE__ , __LINE__