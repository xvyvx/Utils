#if defined(USE_ODBC_DATABASE_UTILS) || defined(USE_OCI_DATABASE_UTILS)

#include "SqlDatabasePool.h"
#include "../ODBC/ODBCSqlDatabase.h"
#include "../OCI/OCISqlDatabase.h"
#include "../../../Common/ObjectPoolBaseImpl.h"

const char SqlDatabasePoolLoggerName[] = "SqlDatabasePool";

ISqlDatabase* SqlDatabaseFactory::operator()(SqlDatabaseType type)
{
	ISqlDatabase *ret = 0;
	switch(type)
	{
	case SqlDatabaseType::ODBC:
#if defined(USE_ODBC_DATABASE_UTILS)
		ret = new ODBCSqlDatabase(SqlDatabaseFactory::Instance.m_connStr, true);
#endif
		break;
	case SqlDatabaseType::OCI:
#if defined(USE_OCI_DATABASE_UTILS)
		ret = new OCISqlDatabase(SqlDatabaseFactory::Instance.m_connStr, true);
#endif
		break;
	}
	return ret;
}

class OtlInitialize
{
public:
	OtlInitialize()
	{
#if defined(OTL_DB2_CLI)
		db2::otl_connect::otl_initialize(1);
#elif  defined(OTL_ODBC)||defined(OTL_ODBC_MYSQL)
		odbc::otl_connect::otl_initialize(1);
#endif

#if defined(OTL_ORA8)||defined(OTL_ORA8I)||defined(OTL_ORA9I)||defined(OTL_ORA10G)||defined(OTL_ORA10G_R2)||defined(OTL_ORA11G)||\
	defined(OTL_ORA11G_R2)||defined(OTL_ORA12C)
		oracle::otl_connect::otl_initialize(1);
#endif
	}

	~OtlInitialize()
	{
#if defined(OTL_ORA8)||defined(OTL_ORA8I)||defined(OTL_ORA9I)||defined(OTL_ORA10G)||defined(OTL_ORA10G_R2)||defined(OTL_ORA11G)||\
	defined(OTL_ORA11G_R2)||defined(OTL_ORA12C)
		oracle::otl_connect::otl_terminate();
#endif
	}
} OtlInitializer;

#endif