#ifndef __CS_GENERATOR_H__
#define __CS_GENERATOR_H__

#include "CodeGenerator.h"

class CSGenerator : public CodeGenerator
{
public:
	CSGenerator(){gens_[type()] = this;}
public:
	virtual const char* name(){return "cs";};
	virtual CodeGenType type(){return T_Cs;};
	virtual void generate();
	virtual void accept(Enum *node){};
	virtual void accept(Struct*node){};
	virtual	void accept(Service*node){};
};

#endif ///__CS_GENERATOR_H__