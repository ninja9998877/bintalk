#ifndef __Generator_h__
#define __Generator_h__

#include <string>
class Enum;
class Struct;
class Service;

enum CodeGenType
{
	T_Cpp,
	T_Cs,
	T_Erl,
	T_Py,
	T_Max
};

class CodeGenerator
{
public:
	virtual const char* name()=0;
	virtual CodeGenType type()=0;
	virtual void generate()=0;
	virtual void accept(Enum *node){};
	virtual void accept(Struct*node){};
	virtual	void accept(Service*node){};
public:
	static const char* desc();
	static bool exec();
protected:
	static CodeGenerator* gens_[T_Max];
};

#endif // __Generator_h__
