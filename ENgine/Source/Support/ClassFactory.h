
#pragma once

#define CLASSFACTORYDEF(baseClass) \
class ClassFactory##baseClass \
{\
public:\
	ClassFactory##baseClass()\
	{\
		Decls().push_back(this);\
	};\
	static void Sort()\
	{\
		for (int i=0; i<Decls().size() - 1; i++)\
		{\
			for (int j = i + 1; j < Decls().size(); j++)\
			{\
				if (!StringUtils::CompareABC(Decls()[i]->GetName(), Decls()[j]->GetName()))\
				{\
					ClassFactory##baseClass* tmp = Decls()[i];\
					Decls()[i] = Decls()[j];\
					Decls()[j] = tmp;\
				}\
			}\
		}\
	}\
	virtual ~ClassFactory##baseClass() {};\
	static ClassFactory##baseClass* Find(const char* name)\
	{\
		for (auto& decl : Decls())\
		{\
			if (StringUtils::IsEqual(decl->GetName(), name) ||\
				StringUtils::IsEqual(decl->GetShortName(), name))\
			{\
				return decl;\
			}\
		}\
		return nullptr;\
	}\
	virtual const char* GetName() = 0;\
	virtual const char* GetShortName() = 0;\
	virtual baseClass* Create(const char* file, int line) = 0;\
	static baseClass* Create(const char* name, const char* file, int line)\
	{\
		ClassFactory##baseClass* decl = Find(name);\
		if (decl)\
		{\
			return decl->Create(file, line);\
		}\
		return nullptr;\
	}\
	static eastl::vector<ClassFactory##baseClass*>& Decls()\
	{\
		static eastl::vector<ClassFactory##baseClass*> decls;\
		return decls;\
	}

#define CLASSFACTORYDEF_END()\
};

#define CLASSREGEX(baseClass, shortClassName, fullClassName, shortName)\
class ClassFactory##shortClassName##baseClass : public ClassFactory##baseClass\
{\
public:\
	virtual const char* GetName() { return #shortClassName; };\
	virtual const char* GetShortName() { return shortName; };\
	virtual baseClass* Create(const char* file, int line) { return new(file, line) fullClassName(); };

#define CLASSREGEX_END(baseClass, shortClassName)\
};\
ClassFactory##shortClassName##baseClass classFactory##shortClassName##baseClass;

#define CLASSREG(baseClass, className, shortName)\
CLASSREGEX(baseClass, className, className, shortName)\
CLASSREGEX_END(baseClass, className)