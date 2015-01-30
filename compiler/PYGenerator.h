#ifndef __PY_GENERATOR_H__
#define __PY_GENERATOR_H__

#include "CodeGenerator.h"

class PYGenerator : public CodeGenerator
{
public:
	PYGenerator(){gens_[type()] = this;}
public:
	virtual const char* name(){return "py";};
	virtual CodeGenType type(){return T_Py;};
	virtual void generate();
	virtual void accept(Enum *node){};
	virtual void accept(Struct*node){};
	virtual	void accept(Service*node){};
};

#endif ///__PY_GENERATOR_H__