#ifndef SQLDATABASEPOOL_H
#define SQLDATABASEPOOL_H

#include "../../../Common/ObjectPoolBase.h"
#include "SqlDatabaseType.h"
#include "ISqlDatabase.h"

extern const char SqlDatabasePoolLoggerName[];

inline void DummyClearFunc(ISqlDatabase *obj)
{
}

inline bool SqlDatabasePoolPredicatorFunc(ISqlDatabase *obj,SqlDatabaseType type)
{
	return obj->Type() == type;
}

class SqlDatabaseFactory
{
public:
	ISqlDatabase* operator()(SqlDatabaseType type);

	const char *m_connStr;

	static SqlDatabaseFactory Instance;
};

class UTILS_EXPORTS_API SqlDatabsePool :public ObjectPoolBase
	<
	SqlDatabaseType,
	ISqlDatabase,
	SqlDatabsePool,
	SqlDatabaseFactory, &SqlDatabaseFactory::Instance,
	void(ISqlDatabase*), DummyClearFunc,
	bool(ISqlDatabase*, SqlDatabaseType), SqlDatabasePoolPredicatorFunc,
	SqlDatabasePoolLoggerName
	>
{
	friend class ObjectPoolBase
		<
		SqlDatabaseType,
		ISqlDatabase,
		SqlDatabsePool,
		SqlDatabaseFactory, &SqlDatabaseFactory::Instance,
		void(ISqlDatabase*), DummyClearFunc,
		bool(ISqlDatabase*, SqlDatabaseType), SqlDatabasePoolPredicatorFunc,
		SqlDatabasePoolLoggerName
		>;
};

#endif /* SQLDATABASEPOOL_H */