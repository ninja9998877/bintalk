#include <cstdio>
#include "CodeGenerator.h"
#include "Options.h"

CodeGenerator* CodeGenerator::gens_[T_Max] = {NULL};

const char* CodeGenerator::desc()
{
	static std::string desc;
	desc = "available code generators(";
	
	for(int i=0; i<T_Max; ++i)
	{
		desc += " ";
		desc += gens_[i]->name();
	}
	desc += " )";
	return desc.c_str();
}

bool CodeGenerator::exec()
{
	CodeGenerator *cg = NULL;
	for(int i=0; i<T_Max; ++i)
	{
		if(gOptions.generator_ == gens_[i]->name())
		{
			cg = gens_[i];
			break;
		}
	}

	if(cg == NULL)
	{
		fprintf(stderr, "invalid generator \"%s\"\n", gOptions.generator_.c_str());
		return false;
	}
	try
	{
		cg->generate();
	}
	catch (const char* e)
	{
		fprintf(stderr, "%s\n", e);
		return false;
	}
	return true;
}
