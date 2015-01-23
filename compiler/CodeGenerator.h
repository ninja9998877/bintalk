#ifndef __Generator_h__
#define __Generator_h__

#include <string>
class Enum;
class Struct;
class Service;
class CodeGenerator
{
public:
	CodeGenerator(const std::string& name);
	virtual void generate()=0;
private:
	std::string		name_;
	CodeGenerator*	next_;
public:
	virtual void accept(Enum *node){};
	virtual void accept(Struct*node){};
	virtual	void accept(Service*node){};
public:
	static const char* desc();
	static bool exec();

private:
	static CodeGenerator* root_;
};

#define DECLARE_CG(CLASS, NAME) \
	class CLASS : public CodeGenerator\
	{public:\
		CLASS():CodeGenerator(#NAME){}\
		virtual void generate();\
	};\
	CLASS __##NAME##__;

#endif // __Generator_h__
