#ifdef USE_OCI_DATABASE_UTILS

#include "OCISqlDatabase.h"
#include <boost/lexical_cast.hpp>

OracleSqlDatabase::OracleSqlDatabase(const char *connStr, bool connect, int cmdPoolSize) :m_connection(), m_cmd(), m_refCur(), m_connStr(connStr)
	, m_lastError(), m_cmdStayInPool(true)
{
	m_lastError.reserve(4096);
	m_connection.set_stream_pool_size(cmdPoolSize);
	if (connect)
	{
		try
		{
			m_connection.rlogon(m_connStr.c_str(), false);
		}
		catch (const oracle::otl_exception &err)
		{
			FormatExceptionMsg(err, m_lastError, "构造连接错误", true);
		}
	}
}

OracleSqlDatabase::~OracleSqlDatabase()
{
}

SqlDatabaseType OracleSqlDatabase::Type()
{
	return ::RISEComm::MST::Framework::Database::Oracle;
}

bool OracleSqlDatabase::Prepare(const char *sql, int bufferSize, bool stayInPool, bool storedPorcReturnDataset)
{
	bool ret = true;
	try
	{
		if (!m_connection.connected)
		{
			m_connection.rlogon(m_connStr.c_str(), false);
		}
		if(m_refCur)
		{
			m_refCur.reset();
		}
		if (m_cmd.good())
		{
			try
			{
				m_cmd.close(m_cmdStayInPool);
			}
			catch (...)
			{
			}
		}
		m_cmd.open(bufferSize + 1, sql, m_connection);//otl在buffer满时会自动flush，所以这里加1，防止最后一条记录放入buffer后自动执行
		if(storedPorcReturnDataset)
		{
			m_refCur.reset(new oracle::otl_refcur_stream());
		}
		m_cmdStayInPool = stayInPool;
		m_cmd.set_commit(false);
		m_cmd.set_flush(false);
	}
	catch (const oracle::otl_exception &err)
	{
		ret = false;
		FormatExceptionMsg(err, m_lastError, "准备SQL错误");
		HandleNetworkError(err);
	}
	return ret;
}

bool OracleSqlDatabase::FlushBuffer()
{
	bool ret = true;
	try
	{
		m_cmd.flush();
		if(m_refCur)
		{
			m_cmd >> m_refCur;
		}
	}
	catch (const oracle::otl_exception &err)
	{
		ret = false;
		FormatExceptionMsg(err, m_lastError, "执行SQL错误");
		HandleNetworkError(err);
	}
	return ret;
}

bool OracleSqlDatabase::Commit()
{
	bool ret = true;
	try
	{
		m_connection.commit();
	}
	catch (const oracle::otl_exception &err)
	{
		ret = false;
		FormatExceptionMsg(err, m_lastError, "提交SQL错误");
		HandleNetworkError(err);
	}
	return ret;
}

bool OracleSqlDatabase::Rollback()
{
	bool ret = true;
	try
	{
		m_connection.rollback();
	}
	catch (const oracle::otl_exception &err)
	{
		ret = false;
		FormatExceptionMsg(err, m_lastError, "回滚SQL错误");
		HandleNetworkError(err);
	}
	return ret;
}

int OracleSqlDatabase::IsEOF()
{
	try
	{
		if(m_refCur)
		{
			return m_refCur->eof();
		}
		else
		{
			return m_cmd.eof();
		}
	}
	catch (const oracle::otl_exception &err)
	{
		FormatExceptionMsg(err, m_lastError, "获取查询结束标志错误");
		HandleNetworkError(err);
	}
	return -1;
}

void OracleSqlDatabase::HandleNetworkError(const oracle::otl_exception &err)
{
	if (IsNetworkError(err))
	{
		try
		{
			if(m_refCur)
			{
				m_refCur.reset();
			}
			try
			{
				m_cmd.close(m_cmdStayInPool);
			}
			catch (...)
			{
			}
			m_connection.logoff();
		}
		catch (const oracle::otl_exception&)
		{
		}
	}
}

const std::string& OracleSqlDatabase::LastError()
{
	return m_lastError;
}

ISqlDatabase& OracleSqlDatabase::operator<<(const unsigned char* s)
{
	try
	{
		m_cmd << s;
		return *this;
	}
	catch (const oracle::otl_exception &err)
	{
		FormatExceptionMsg(err, m_lastError, "设置参数错误");
		HandleNetworkError(err);
		throw;
	}
}

ISqlDatabase& OracleSqlDatabase::operator<<(const char c)
{
	try
	{
		m_cmd << c;
		return *this;
	}
	catch (const oracle::otl_exception &err)
	{
		FormatExceptionMsg(err, m_lastError, "设置参数错误");
		HandleNetworkError(err);
		throw;
	}
}

ISqlDatabase& OracleSqlDatabase::operator<<(const char* s)
{
	try
	{
		m_cmd << s;
		return *this;
	}
	catch (const oracle::otl_exception &err)
	{
		FormatExceptionMsg(err, m_lastError, "设置参数错误");
		HandleNetworkError(err);
		throw;
	}
}

ISqlDatabase& OracleSqlDatabase::operator<<(const otl_long_string& d)
{
	try
	{
		m_cmd << d;
		return *this;
	}
	catch (const oracle::otl_exception &err)
	{
		FormatExceptionMsg(err, m_lastError, "设置参数错误");
		HandleNetworkError(err);
		throw;
	}
}

ISqlDatabase& OracleSqlDatabase::operator<<(const std::string& s)
{
	try
	{
		m_cmd << s;
		return *this;
	}
	catch (const oracle::otl_exception &err)
	{
		FormatExceptionMsg(err, m_lastError, "设置参数错误");
		HandleNetworkError(err);
		throw;
	}
}

ISqlDatabase& OracleSqlDatabase::operator<<(const int n)
{
	try
	{
		m_cmd << n;
		return *this;
	}
	catch (const oracle::otl_exception &err)
	{
		FormatExceptionMsg(err, m_lastError, "设置参数错误");
		HandleNetworkError(err);
		throw;
	}
}

ISqlDatabase& OracleSqlDatabase::operator<<(const unsigned int u)
{
	try
	{
		m_cmd << u;
		return *this;
	}
	catch (const oracle::otl_exception &err)
	{
		FormatExceptionMsg(err, m_lastError, "设置参数错误");
		HandleNetworkError(err);
		throw;
	}
}

ISqlDatabase& OracleSqlDatabase::operator<<(const short sh)
{
	try
	{
		m_cmd << sh;
		return *this;
	}
	catch (const oracle::otl_exception &err)
	{
		FormatExceptionMsg(err, m_lastError, "设置参数错误");
		HandleNetworkError(err);
		throw;
	}
}

ISqlDatabase& OracleSqlDatabase::operator<<(const long int l)
{
	try
	{
		m_cmd << l;
		return *this;
	}
	catch (const oracle::otl_exception &err)
	{
		FormatExceptionMsg(err, m_lastError, "设置参数错误");
		HandleNetworkError(err);
		throw;
	}
}

ISqlDatabase& OracleSqlDatabase::operator<<(const us64 l)
{
	try
	{
		m_cmd << l;
		return *this;
	}
	catch (const oracle::otl_exception &err)
	{
		FormatExceptionMsg(err, m_lastError, "设置参数错误");
		HandleNetworkError(err);
		throw;
	}
}

ISqlDatabase& OracleSqlDatabase::operator<<(const s64 l)
{
	try
	{
		m_cmd << l;
		return *this;
	}
	catch (const oracle::otl_exception &err)
	{
		FormatExceptionMsg(err, m_lastError, "设置参数错误");
		HandleNetworkError(err);
		throw;
	}
}

ISqlDatabase& OracleSqlDatabase::operator<<(const float f)
{
	try
	{
		m_cmd << f;
		return *this;
	}
	catch (const oracle::otl_exception &err)
	{
		FormatExceptionMsg(err, m_lastError, "设置参数错误");
		HandleNetworkError(err);
		throw;
	}
}

ISqlDatabase& OracleSqlDatabase::operator<<(const double d)
{
	try
	{
		m_cmd << d;
		return *this;
	}
	catch (const oracle::otl_exception &err)
	{
		FormatExceptionMsg(err, m_lastError, "设置参数错误");
		HandleNetworkError(err);
		throw;
	}
}

ISqlDatabase& OracleSqlDatabase::operator<<(const otl_null &n)
{
	try
	{
		m_cmd << n;
		return *this;
	}
	catch (const oracle::otl_exception &err)
	{
		FormatExceptionMsg(err, m_lastError, "设置参数错误");
		HandleNetworkError(err);
		throw;
	}
}

ISqlDatabase& OracleSqlDatabase::operator<<(const otl_datetime& dt)
{
	try
	{
		m_cmd << dt;
		return *this;
	}
	catch (const oracle::otl_exception &err)
	{
		FormatExceptionMsg(err, m_lastError, "设置参数错误");
		HandleNetworkError(err);
		throw;
	}
}

ISqlDatabase& OracleSqlDatabase::operator >> (unsigned char* s)
{
	try
	{
		if(m_refCur)
		{
			(*m_refCur) >> s;
		}
		else
		{
			m_cmd >> s;
		}
		return *this;
	}
	catch (const oracle::otl_exception &err)
	{
		FormatExceptionMsg(err, m_lastError, "读取查询结果错误");
		HandleNetworkError(err);
		throw;
	}
}

ISqlDatabase& OracleSqlDatabase::operator >> (char& c)
{
	try
	{
		if (m_refCur)
		{
			(*m_refCur) >> c;
		}
		else
		{
			m_cmd >> c;
		}
		return *this;
	}
	catch (const oracle::otl_exception &err)
	{
		FormatExceptionMsg(err, m_lastError, "读取查询结果错误");
		HandleNetworkError(err);
		throw;
	}
}

ISqlDatabase& OracleSqlDatabase::operator >> (unsigned char& c)
{
	try
	{
		if (m_refCur)
		{
			(*m_refCur) >> c;
		}
		else
		{
			m_cmd >> c;
		}
		return *this;
	}
	catch (const oracle::otl_exception &err)
	{
		FormatExceptionMsg(err, m_lastError, "读取查询结果错误");
		HandleNetworkError(err);
		throw;
	}
}

ISqlDatabase& OracleSqlDatabase::operator >> (char* s)
{
	try
	{
		if (m_refCur)
		{
			(*m_refCur) >> s;
		}
		else
		{
			m_cmd >> s;
		}
		return *this;
	}
	catch (const oracle::otl_exception &err)
	{
		FormatExceptionMsg(err, m_lastError, "读取查询结果错误");
		HandleNetworkError(err);
		throw;
	}
}

ISqlDatabase& OracleSqlDatabase::operator >> (otl_long_string& s)
{
	try
	{
		if (m_refCur)
		{
			(*m_refCur) >> s;
		}
		else
		{
			m_cmd >> s;
		}
		return *this;
	}
	catch (const oracle::otl_exception &err)
	{
		FormatExceptionMsg(err, m_lastError, "读取查询结果错误");
		HandleNetworkError(err);
		throw;
	}
}

ISqlDatabase& OracleSqlDatabase::operator >> (std::string& s)
{
	try
	{
		if (m_refCur)
		{
			(*m_refCur) >> s;
		}
		else
		{
			m_cmd >> s;
		}
		return *this;
	}
	catch (const oracle::otl_exception &err)
	{
		FormatExceptionMsg(err, m_lastError, "读取查询结果错误");
		HandleNetworkError(err);
		throw;
	}
}

ISqlDatabase& OracleSqlDatabase::operator >> (int& n)
{
	try
	{
		if (m_refCur)
		{
			(*m_refCur) >> n;
		}
		else
		{
			m_cmd >> n;
		}
		return *this;
	}
	catch (const oracle::otl_exception &err)
	{
		FormatExceptionMsg(err, m_lastError, "读取查询结果错误");
		HandleNetworkError(err);
		throw;
	}
}

ISqlDatabase& OracleSqlDatabase::operator >> (unsigned int& u)
{
	try
	{
		if (m_refCur)
		{
			(*m_refCur) >> u;
		}
		else
		{
			m_cmd >> u;
		}
		return *this;
	}
	catch (const oracle::otl_exception &err)
	{
		FormatExceptionMsg(err, m_lastError, "读取查询结果错误");
		HandleNetworkError(err);
		throw;
	}
}

ISqlDatabase& OracleSqlDatabase::operator >> (short& sh)
{
	try
	{
		if (m_refCur)
		{
			(*m_refCur) >> sh;
		}
		else
		{
			m_cmd >> sh;
		}
		return *this;
	}
	catch (const oracle::otl_exception &err)
	{
		FormatExceptionMsg(err, m_lastError, "读取查询结果错误");
		HandleNetworkError(err);
		throw;
	}
}

ISqlDatabase& OracleSqlDatabase::operator >> (long int& l)
{
	try
	{
		if (m_refCur)
		{
			(*m_refCur) >> l;
		}
		else
		{
			m_cmd >> l;
		}
		return *this;
	}
	catch (const oracle::otl_exception &err)
	{
		FormatExceptionMsg(err, m_lastError, "读取查询结果错误");
		HandleNetworkError(err);
		throw;
	}
}

ISqlDatabase& OracleSqlDatabase::operator >> (float& f)
{
	try
	{
		if (m_refCur)
		{
			(*m_refCur) >> f;
		}
		else
		{
			m_cmd >> f;
		}
		return *this;
	}
	catch (const oracle::otl_exception &err)
	{
		FormatExceptionMsg(err, m_lastError, "读取查询结果错误");
		HandleNetworkError(err);
		throw;
	}
}

ISqlDatabase& OracleSqlDatabase::operator >> (double& d)
{
	try
	{
		if (m_refCur)
		{
			(*m_refCur) >> d;
		}
		else
		{
			m_cmd >> d;
		}
		return *this;
	}
	catch (const oracle::otl_exception &err)
	{
		FormatExceptionMsg(err, m_lastError, "读取查询结果错误");
		HandleNetworkError(err);
		throw;
	}
}

ISqlDatabase& OracleSqlDatabase::operator >> (otl_datetime& dt)
{
	try
	{
		if (m_refCur)
		{
			(*m_refCur) >> dt;
		}
		else
		{
			m_cmd >> dt;
		}
		return *this;
	}
	catch (const oracle::otl_exception &err)
	{
		FormatExceptionMsg(err, m_lastError, "读取查询结果错误");
		HandleNetworkError(err);
		throw;
	}
}

void OracleSqlDatabase::FormatExceptionMsg(const oracle::otl_exception &ex, std::string& msg, const char *context, bool onlyMsg)
{
	msg.clear();
	msg.append(context);
	msg.append("，错误信息：");
	msg.append(reinterpret_cast<const char*>(ex.msg));
	if (!onlyMsg)
	{
		msg.append("\r\nsql语句：");
		msg.append(ex.stm_text);
		msg.append("\r\n错误代码：");
		msg.append(boost::lexical_cast<std::string>(ex.code));
		msg.append("\r\n错误偏移：");
		msg.append(boost::lexical_cast<std::string>(ex.error_offset));
	}
}

bool OracleSqlDatabase::IsNetworkError(const oracle::otl_exception &ex)
{
#error Not implement.
}

#endif