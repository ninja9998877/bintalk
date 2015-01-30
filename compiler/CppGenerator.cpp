#include "Options.h"
#include "Context.h"
#include "CodeFile.h"
#include "CppGenerator.h"

#include "Options.h"
#include "Context.h"
#include "CodeFile.h"
#include "CppGenerator.h"

static CppGenerator gen__inside;

const char* CppGenerator::getFieldCppType(Field& f)
{
	static std::string name;
	switch(f.type_)
	{
	case Field::FT_INT64:	name = "bintalk::INT64"; break;
	case Field::FT_UINT64: name = "bintalk::UINT64"; break;
	case Field::FT_DOUBLE: name = "bintalk::DOUBLE"; break;
	case Field::FT_FLOAT:	name = "bintalk::FLOAT"; break;
	case Field::FT_INT32:	name = "bintalk::INT32"; break;
	case Field::FT_UINT32: name = "bintalk::UINT32"; break;
	case Field::FT_INT16:	name = "bintalk::INT16"; break;
	case Field::FT_UINT16: name = "bintalk::UINT16"; break;
	case Field::FT_INT8:	name = "bintalk::INT8"; break;
	case Field::FT_UINT8:	name = "bintalk::UINT8"; break;
	case Field::FT_BOOL:	name = "bintalk::BOOL"; break;
	case Field::FT_STRING: name = "bintalk::STRING"; break;
	case Field::FT_BINARY: name = "bintalk::BINARY"; break;
	case Field::FT_ENUM:
	case Field::FT_USER:	name = f.userType_->name_; break;
	}
	if(f.isArray()) name = "std::vector<" + name + ">";
	return name.c_str();
}

const char* CppGenerator::getFieldCppParamType(Field& f, bool isS)
{
	static std::string name;
	name = getFieldCppType(f);
	if(f.isArray() || f.type_ == Field::FT_STRING || f.type_ == Field::FT_BINARY || f.type_ == Field::FT_USER)
	{
		if(isS) name = "const " + name;
		name += "&";
	}
	return name.c_str();
}

const char* CppGenerator::getFieldCppDefault(Field& f)
{
	static std::string name;
	if(f.isArray())
		return NULL;
	name = "";
	switch(f.type_)
	{
	case Field::FT_INT64:
	case Field::FT_UINT64:
	case Field::FT_DOUBLE:
	case Field::FT_FLOAT:
	case Field::FT_INT32:
	case Field::FT_UINT32:
	case Field::FT_INT16:
	case Field::FT_UINT16:
	case Field::FT_INT8:
	case Field::FT_UINT8:	name = "0"; break;
	case Field::FT_BOOL:	name = "false"; break;
	case Field::FT_ENUM:	name = "(" + f.userType_->name_ + ")(0)"; break;
	case Field::FT_STRING:
	case Field::FT_BINARY:
	case Field::FT_USER: return NULL;
	}
	return name.c_str();
}

const char* CppGenerator::getFieldFuncName(Field& f)
{
	static std::string n;
	switch(f.type_)
	{
	case Field::FT_INT64:	n = "INT64";break;
	case Field::FT_UINT64:	n = "UINT64";break;
	case Field::FT_DOUBLE: n = "DOUBLE";break;
	case Field::FT_FLOAT:	n = "FLOAT";break;
	case Field::FT_INT32:	n = "INT32";break;
	case Field::FT_UINT32: n = "UINT32";break;
	case Field::FT_INT16:	n = "INT16";break;
	case Field::FT_UINT16:	n = "UINT16";break;
	case Field::FT_INT8:	n = "INT8";break;
	case Field::FT_UINT8:	n = "UINT8";break;
	case Field::FT_BOOL:	n = "BOOL";break;
	case Field::FT_ENUM:	n = "ENUM";break;
	case Field::FT_STRING:	n = "STRING";break;
	case Field::FT_BINARY:	n = "BINARY";break;
	case Field::FT_USER:	n = "USER";break;
	}
	if(f.isArray()) n += "A";
	return n.c_str();
}

void CppGenerator::generateStubMethodDecl(CodeFile& f, Method& m)
{
	f.listBegin(",", false, "void %s(", m.getNameC());
	for(size_t i = 0; i < m.fields_.size(); i++)
	{
		Field& field = m.fields_[i];
		f.listItem("%s %s", getFieldCppParamType(field, true), field.getNameC());
	}
	f.listEnd(");");
}

void CppGenerator::generateProxyMethodDecl(CodeFile& f, Method& m)
{
	f.listBegin(",", false, "virtual bool %s(", m.getNameC());
	for(size_t i = 0; i < m.fields_.size(); i++)
	{
		Field& field = m.fields_[i];
		f.listItem("%s %s", getFieldCppParamType(field, false), field.getNameC());
	}
	f.listEnd(") = 0;");
}

void CppGenerator::generateStructDecl(CodeFile& f, Struct* s)
{
	f.output("struct %s", s->getNameC());
	f.indent("{");
	for(size_t i = 0; i < s->fields_.size(); i++)
	{
		Field& field = s->fields_[i];
		f.output("%s %s;", getFieldCppType(field), field.getNameC());
	}
	f.output("%s();", s->getNameC());
	f.output("void serialize(bintalk::BinaryWriter* w) const;");
	f.output("bool deserialize(bintalk::BinaryReader* r);");
	f.recover("};");
}

void CppGenerator::generateStubDecl(CodeFile& f, Service* s)
{
	f.output("class %sStub", s->getNameC());
	f.indent("{");
	f.output("public:");
	for(size_t i = 0; i < s->methods_.size(); i++)
		generateStubMethodDecl(f, s->methods_[i]);
	f.output("protected:");
	f.output("virtual bintalk::BinaryWriter* methodBegin() = 0;");
	f.output("virtual void methodEnd() = 0;");
	f.recover("};");
}

void CppGenerator::generateProxyDecl(CodeFile& f, Service* s)
{
	f.output("class %sProxy", s->getNameC());
	f.indent("{");
	f.output("public:");
	f.output("typedef class %sDispatcher Dispatcher;", s->getNameC());
	// Generate methed id.
	f.output("enum MID");
	f.indent("{");
	for(size_t i = 0; i < s->methods_.size(); i++)
	{
		Method& method = s->methods_[i];
		f.output("MID_%s = %d,", method.name_.c_str(), method.mid_);
	}
	f.recover("};");
	f.output("virtual bool filterMethod(%sProxy::MID mid) { return true; }", s->getNameC());
	for(size_t i = 0; i < s->methods_.size(); i++)	
		generateProxyMethodDecl(f, s->methods_[i]);
	f.recover("};");
}

void CppGenerator::generateDispatcherDecl(CodeFile& f, Service* s)
{
	f.output("class %sDispatcher", s->getNameC());
	f.indent("{");
	f.output("public:");
	f.output("typedef class %sProxy Proxy;", s->getNameC());
	f.output("static bool dispatch(bintalk::BinaryReader* reader, %sProxy* p);", s->getNameC());
	f.output("protected:");
	for(size_t i = 0; i < s->methods_.size(); i++)
		f.output("static bool %s(bintalk::BinaryReader* r, %sProxy* p);", s->methods_[i].getNameC(), s->getNameC());
	f.recover("};");
}

void CppGenerator::generateFieldContainerSCode(CodeFile& f, FieldContainer* fc)
{
	for(size_t i = 0; i < fc->fields_.size(); i++)
	{
		Field& field = fc->fields_[i];
		f.output("bintalk::ProtocolWriter::write%s(__w__, %s);",
			getFieldFuncName(field),
			field.getNameC());
	}
}

void CppGenerator::generateFieldContainerDSCode(CodeFile& f, FieldContainer* fc)
{
	for(size_t i = 0; i < fc->fields_.size(); i++)
	{
		Field& field = fc->fields_[i];
		f.output("if(!bintalk::ProtocolReader::read%s(__r__, %s, 0X%X, 0X%X)) return false;",
			getFieldFuncName(field),
			field.getNameC(),
			field.maxArray_,
			field.maxValue_);
	}
}

void CppGenerator::generateStructImp(CodeFile& f, Struct* s)
{
	f.output("%s::%s()", s->getNameC(), s->getNameC());
	f.indent("{");
	for(size_t i = 0; i < s->fields_.size(); i++)
	{
		Field& field = s->fields_[i];
		if(!getFieldCppDefault(field))
			continue;
		f.output("%s = %s;", field.getNameC(), getFieldCppDefault(field));
	}
	f.recover("}");
	f.output("void %s::serialize(bintalk::BinaryWriter* __w__) const", s->getNameC());
	f.indent("{");
	generateFieldContainerSCode(f, s);
	f.recover("}");
	f.output("bool %s::deserialize(bintalk::BinaryReader* __r__)", s->getNameC());
	f.indent("{");
	generateFieldContainerDSCode(f, s);
	f.output("return true;");
	f.recover("}");
}

void CppGenerator::generateStubMethodImp(CodeFile& f, Service*s, Method& m)
{
	f.listBegin(",", false, "void %sStub::%s(", s->getNameC(), m.getNameC());
	for(size_t i = 0; i < m.fields_.size(); i++)
	{
		Field& field = m.fields_[i];
		f.listItem("%s %s", getFieldCppParamType(field, true), field.getNameC());
	}
	f.listEnd(")");
	f.indent("{");
	f.output("bintalk::BinaryWriter* __w__ = methodBegin();");
	f.output("bintalk::MID __mid__ = %d;", m.mid_);
	f.output("bintalk::ProtocolWriter::writeMID(__w__, __mid__);");
	generateFieldContainerSCode(f, &m);
	f.output("methodEnd();");
	f.recover("}");
}

void CppGenerator::generateStubImp(CodeFile& f, Service* s)
{
	for(size_t i = 0; i < s->methods_.size(); i++)
		generateStubMethodImp(f, s, s->methods_[i]);
}

void CppGenerator::generateDispatcherMethodImp(CodeFile&f, Method& m, const std::string& svcName)
{
	f.output("bool %sDispatcher::%s(bintalk::BinaryReader* __r__, %sProxy* __p__)", svcName.c_str(), m.getNameC(), svcName.c_str());
	f.indent("{");
	for(size_t i = 0; i < m.fields_.size(); i++)
	{
		Field& field = m.fields_[i];
		if(getFieldCppDefault(field))
			f.output("%s %s=%s;", getFieldCppType(field), field.getNameC(), getFieldCppDefault(field));
		else
			f.output("%s %s;", getFieldCppType(field), field.getNameC());
	}
	generateFieldContainerDSCode(f, &m);
	f.listBegin(",", false, "return __p__->%s(", m.getNameC());
	for(size_t i = 0; i < m.fields_.size(); i++)
	{
		Field& field = m.fields_[i];
		f.listItem("%s", field.getNameC());
	}
	f.listEnd(");");
	f.recover("}");
}

void CppGenerator::generateDispatcherImp(CodeFile& f, Service* s)
{
	for(size_t i = 0; i < s->methods_.size(); i++)
		generateDispatcherMethodImp(f, s->methods_[i], s->name_);

	f.output("bool %sDispatcher::dispatch(bintalk::BinaryReader* __r__, %sProxy* __p__)", s->getNameC(), s->getNameC());
	f.indent("{");
	f.output("bintalk::MID __mid__;");
	f.output("if(!bintalk::ProtocolReader::readMID(__r__, __mid__)) return false;");
	f.output("if(__p__->filterMethod((%sProxy::MID)__mid__)) return false;", s->getNameC());
	f.output("switch(__mid__)");
	f.indent("{");
	for(size_t i = 0; i < s->methods_.size(); i++)
	{
		Method& method = s->methods_[i];
		f.output("case %d:{if(!%s(__r__, __p__)) return false;}break;", i, method.getNameC());
	}
	f.output("default: return false;");
	f.recover("}");
	f.output("return true;");
	f.recover("}");
}

void CppGenerator::generate()
{
	std::string fn = gOptions.output_ + "bintalk.h";
	CodeFile f(fn);
	
	f.output("#ifndef __BINTALK_H__", gOptions.inputFS_.c_str());
	f.output("#define __BINTALK_H__", gOptions.inputFS_.c_str());
	f.output("#include \"bintalk/Common.h\"");
	f.output("#include \"bintalk/ProtocolReader.h\"");
	f.output("#include \"bintalk/ProtocolWriter.h\"");
	f.output("#include \"bintalk/FileReader.h\"");
	f.output("#include \"bintalk/FileWriter.h\"");
	f.output("#include \"bintalk/MemoryReader.h\"");
	f.output("#include \"bintalk/MemoryWriter.h\"");
	f.output("#include \"bintalk/BinaryReader.h\"");
	f.output("#include \"bintalk/BinaryWriter.h\"");
	f.output("");
	for(size_t i = 0; i < gContext.definitions_.size(); i++)
	{
		Definition* definition = gContext.definitions_[i];
		/*if(definition->file_ != gOptions.inputFN_)
			continue;*/

		definition->visit(this);
		std::string incf = "#include \"";
		incf += definition->name_;
		incf += ".h\"";
		f.output(incf.c_str());
	}

	f.output("#endif");
}

void CppGenerator::accept(Enum *node)
{
	std::string fn = gOptions.output_ + node->name_ + ".h";
	CodeFile f(fn);
	f.output("enum %s", node->getNameC());
	f.indent("{");
	for(size_t i = 0; i < node->items_.size(); i++)
		f.output("%s,", node->items_[i].c_str());
	f.recover("};");
	f.split();
	f.output("template<> struct bintalk::info<%s>",node->getNameC());
	f.indent("{");

	f.output("static inline const bintalk::INT8* toString(bintalk::INT32 index)");
	f.indent("{");
	f.output("switch(index)");
	f.indent("{");
	for(size_t i = 0; i < node->items_.size(); i++)
		f.output("case %d: return \"%s\";",i,node->items_[i].c_str());
	f.output("default: return \"UNKNOWN\";");
	f.recover("}");
	f.recover("}");
	f.output("static inline int toIndex(const char* name)");
	f.indent("{");
	f.output("if(NULL == name) return -1;");
	for(size_t i = 0; i < node->items_.size(); i++)
		f.output("else if(strcmp(name,\"%s\") == 0) return %d;",node->items_[i].c_str(),i);
	f.output("else return -1;");
	f.recover("}");
	f.recover("};");
	f.split();
}

void CppGenerator::accept(Struct*node)
{
	std::string fn = gOptions.output_ + node->name_;
	CodeFile h(fn+".h");
	generateStructDecl(h,node);
	CodeFile cpp(fn+".cpp");
	cpp.split();
	cpp.output("#include \"bintalk.h\"");
	cpp.split();
	generateStructImp(cpp,node);
}

void CppGenerator::accept(Service*node)
{
	std::string fn = gOptions.output_ + node->name_;

	CodeFile h(fn+".h");
	h.split();
	generateStubDecl(h,node);
	h.split();
	generateProxyDecl(h,node);
	h.split();
	generateDispatcherDecl(h,node);
	CodeFile cpp(fn+".cpp");
	cpp.split();
	cpp.output("#include \"bintalk.h\"");
	cpp.split();
	generateStubImp(cpp,node);
	cpp.split();
	generateDispatcherImp(cpp,node);
}

