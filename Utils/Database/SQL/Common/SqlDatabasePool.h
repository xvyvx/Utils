#ifndef SQLDATABASEPOOL_H
#define SQLDATABASEPOOL_H

#include "../../../Common/ObjectPoolBase.h"
#include "SqlDatabaseType.h"
#include "ISqlDatabase.h"

extern const char SqlDatabasePoolLoggerName[];

class SqlDatabaseDummyClearFunc
{
public:
    inline void operator()(ISqlDatabase *obj)
    {
    }
};

class SqlDatabaseElementTrait
{
public:
    static SqlDatabaseType GetKey(ISqlDatabase &obj)
    {
        return obj.Type();
    }
};

class SqlDatabaseFactory
{
public:
    ISqlDatabase* CreateObj(SqlDatabaseType type);

    void FreeObj(ISqlDatabase *obj);

    const char *m_connStr;
};

class UTILS_EXPORTS_API SqlDatabsePool :public ObjectPoolBase
    <
    SqlDatabaseType,
    ISqlDatabase,
    SqlDatabaseElementTrait,
    SqlDatabsePool,
    SqlDatabaseFactory, 
    SqlDatabaseDummyClearFunc,
    SqlDatabasePoolLoggerName,
    std::equal_to<SqlDatabaseType>,
    std::equal_to<SqlDatabaseType>
    >
{
    friend class ObjectPoolBase
        <
        SqlDatabaseType,
        ISqlDatabase,
        SqlDatabaseElementTrait,
        SqlDatabsePool,
        SqlDatabaseFactory, 
        SqlDatabaseDummyClearFunc,
        SqlDatabasePoolLoggerName,
        std::equal_to<SqlDatabaseType>,
        std::equal_to<SqlDatabaseType>
        >;
};

#endif /* SQLDATABASEPOOL_H */