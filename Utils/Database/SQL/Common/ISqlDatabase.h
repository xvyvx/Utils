#ifndef IRDATABASE_H
#define IRDATABASE_H

#include <string>
#include "../../../Common/CommonHdr.h"
#include "OTLHdr.h"
#include "SqlDatabaseType.h"

class UTILS_EXPORTS_API ISqlDatabase
{
public:
	ISqlDatabase();

	ISqlDatabase(const ISqlDatabase&) = delete;

	ISqlDatabase(ISqlDatabase&&) = delete;

	virtual ~ISqlDatabase();

	ISqlDatabase& operator=(const ISqlDatabase&) = delete;

	ISqlDatabase& operator=(ISqlDatabase&&) = delete;

	virtual SqlDatabaseType Type() = 0;

	virtual bool Prepare(const char *sql, int bufferSize, bool stayInPool = true, bool storedPorcReturnDataset = false) = 0;

	virtual bool FlushBuffer() = 0;

	virtual bool Commit() = 0;

	virtual bool Rollback() = 0;

	virtual int IsEOF() = 0;

	virtual const std::string& LastError() = 0;

	virtual ISqlDatabase& operator<<(const unsigned char* s) = 0;

	virtual ISqlDatabase& operator<<(const char c) = 0;

	virtual ISqlDatabase& operator<<(const char* s) = 0;

	virtual ISqlDatabase& operator<<(const otl_long_string& d) = 0;

	virtual ISqlDatabase& operator<<(const std::string& s) = 0;

	virtual ISqlDatabase& operator<<(const int n) = 0;

	virtual ISqlDatabase& operator<<(const unsigned int u) = 0;

	virtual ISqlDatabase& operator<<(const short sh) = 0;

	virtual ISqlDatabase& operator<<(const long int l) = 0;

	virtual ISqlDatabase& operator<<(const float f) = 0;

	virtual ISqlDatabase& operator<<(const double d) = 0;

	virtual ISqlDatabase& operator<<(const otl_null &n) = 0;

	virtual ISqlDatabase& operator<<(const otl_datetime& dt) = 0;

	virtual ISqlDatabase& operator>>(unsigned char* s) = 0;

	virtual ISqlDatabase& operator >> (char& c) = 0;

	virtual ISqlDatabase& operator >> (unsigned char& c) = 0;

	virtual ISqlDatabase& operator >> (char* s) = 0;

	virtual ISqlDatabase& operator >> (otl_long_string& s) = 0;

	virtual ISqlDatabase& operator >> (std::string& s) = 0;

	virtual ISqlDatabase& operator >> (int& n) = 0;

	virtual ISqlDatabase& operator >> (unsigned int& u) = 0;

	virtual ISqlDatabase& operator >> (short& sh) = 0;

	virtual ISqlDatabase& operator >> (long int& l) = 0;

	virtual ISqlDatabase& operator >> (float& f) = 0;

	virtual ISqlDatabase& operator >> (double& d) = 0;

	virtual ISqlDatabase& operator >> (otl_datetime& dt) = 0;
};

#endif /* USE_DATABASE_UTILS */