#ifndef __Context_h__
#define __Context_h__

#include <string>
#include <vector>
#include <set>
#include <map>

#include "Definition.h"
#include "Field.h"
#include "Enum.h"
#include "Struct.h"
#include "Service.h"

class Context
{
public:
	Context();
	~Context();

	/** ����Դ�ļ�. */
	bool build();

	/** �������Ʋ���һ������. 
	*/
	Definition* findDefinition(const std::string& name);

	/** �����ǰ�ļ��б������. */
	void error(const char* e, ...);
	
	/** �����Ѿ������������ļ����ƣ���ֹ�ظ�����. */
	std::set<std::string>		imported_;
	/** ���ļ�ֱ�Ӱ������ļ�����. */
	std::vector<std::string>	rootImported_;
	/** ��ǰ���������ļ�����. */
	std::string					curFilename_;
	/** ��ǰ�������ļ����к�. */
	int							lineno_;
	/** ���Դ�ļ�����������arpc����. */
	std::vector<Definition*>	definitions_;
	// ��ǰ����״̬.
	Enum		curEnum_;
	Struct		curStruct_;
	Service		curService_;
	Method		curMethod_;
	Field		curField_;
};

extern Context gContext;

#endif//__Context_h__