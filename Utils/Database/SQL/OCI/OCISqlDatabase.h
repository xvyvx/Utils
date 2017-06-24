#ifndef ORACLERDATABASE_H
#define ORACLERDATABASE_H

#ifdef USE_OCI_DATABASE_UTILS

#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable : 4251)
#endif

#include <memory>
#include "../Common/ISqlDatabase.h"

class UTILS_EXPORTS_API OracleSqlDatabase :public ISqlDatabase
{
public:
	OracleSqlDatabase(const char *connStr, bool connect = false, int cmdPoolSize = 32);

	virtual ~OracleSqlDatabase();

	virtual SqlDatabaseType Type();

	//如果存储过程使用游标返回值，要求有且只有游标这一个输出参数
	virtual bool Prepare(const char *sql, int bufferSize, bool stayInPool = true, bool storedPorcReturnDataset = false);

	virtual bool FlushBuffer();

	virtual bool Commit();

	virtual bool Rollback();

	virtual int IsEOF();

	virtual const std::string& LastError();

	virtual ISqlDatabase& operator<<(const unsigned char* s);

	virtual ISqlDatabase& operator<<(const char c);

	virtual ISqlDatabase& operator<<(const char* s);

	virtual ISqlDatabase& operator<<(const otl_long_string& d);

	virtual ISqlDatabase& operator<<(const std::string& s);

	virtual ISqlDatabase& operator<<(const int n);

	virtual ISqlDatabase& operator<<(const unsigned int u);

	virtual ISqlDatabase& operator<<(const short sh);

	virtual ISqlDatabase& operator<<(const long int l);

	virtual ISqlDatabase& operator<<(const us64 l);

	virtual ISqlDatabase& operator<<(const s64 l);

	virtual ISqlDatabase& operator<<(const float f);

	virtual ISqlDatabase& operator<<(const double d);

	virtual ISqlDatabase& operator<<(const otl_null &n);

	virtual ISqlDatabase& operator<<(const otl_datetime& dt);

	virtual ISqlDatabase& operator >> (unsigned char* s);

	virtual ISqlDatabase& operator >> (char& c);

	virtual ISqlDatabase& operator >> (unsigned char& c);

	virtual ISqlDatabase& operator >> (char* s);

	virtual ISqlDatabase& operator >> (otl_long_string& s);

	virtual ISqlDatabase& operator >> (std::string& s);

	virtual ISqlDatabase& operator >> (int& n);

	virtual ISqlDatabase& operator >> (unsigned int& u);

	virtual ISqlDatabase& operator >> (short& sh);

	virtual ISqlDatabase& operator >> (long int& l);

	virtual ISqlDatabase& operator >> (float& f);

	virtual ISqlDatabase& operator >> (double& d);

	virtual ISqlDatabase& operator >> (otl_datetime& dt);

private:
	OracleSqlDatabase(const OracleSqlDatabase&) = delete;

	OracleSqlDatabase(OracleSqlDatabase&&) = delete;

	OracleSqlDatabase& operator=(const OracleSqlDatabase&) = delete;

	OracleSqlDatabase& operator=(OracleSqlDatabase&&) = delete;

	void HandleNetworkError(const oracle::otl_exception &err);

	static void FormatExceptionMsg(const oracle::otl_exception &ex, std::string &msg, const char *context, bool onlyMsg = false);

	static bool IsNetworkError(const oracle::otl_exception &ex);

	oracle::otl_connect m_connection;

	oracle::otl_stream m_cmd;

	std::unique_ptr<oracle::otl_refcur_stream> m_refCur;

	std::string m_connStr;

	std::string m_lastError;

	bool m_cmdStayInPool;
};

#if defined(_MSC_VER)
	#pragma warning(pop)
#endif

#endif /* USE_OCI_DATABASE_UTILS */

#endif /* ORACLERDATABASE_H */