#ifndef __CPP_GENERATOR_H__
#define __CPP_GENERATOR_H__

#include "CodeGenerator.h"

class CppGenerator : public CodeGenerator
{
public:
	CppGenerator(){gens_[type()] = this;}
public:
	virtual const char* name(){return "cpp";};
	virtual CodeGenType type(){return T_Cpp;};
	const char* getFieldCppType(Field& f);
	const char* getFieldCppParamType(Field& f, bool isS);
	const char* getFieldCppDefault(Field& f);
	const char* getFieldFuncName(Field& f);
	void generateStubMethodDecl(CodeFile& f, Method& m);
	void generateProxyMethodDecl(CodeFile& f, Method& m);
	void generateStructDecl(CodeFile& f, Struct* s);
	void generateStubDecl(CodeFile& f, Service* s);
	void generateProxyDecl(CodeFile& f, Service* s);
	void generateDispatcherDecl(CodeFile& f, Service* s);
	void generateFieldContainerSCode(CodeFile& f, FieldContainer* fc);
	void generateFieldContainerDSCode(CodeFile& f, FieldContainer* fc);
	void generateStructImp(CodeFile& f, Struct* s);
	void generateStubMethodImp(CodeFile& f, Service*s, Method& m);
	void generateStubImp(CodeFile& f, Service* s);
	void generateDispatcherMethodImp(CodeFile&f, Method& m, const std::string& svcName);
	void generateDispatcherImp(CodeFile& f, Service* s);
	void generate();
	void accept(Enum *node);
	void accept(Struct*node);
	void accept(Service*node);

};

#endif ///__CPP_GENERATOR_H__


