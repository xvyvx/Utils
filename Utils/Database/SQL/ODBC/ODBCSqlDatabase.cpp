#ifdef USE_ODBC_DATABASE_UTILS

#include "ODBCSqlDatabase.h"

ODBCSqlDatabase::ODBCSqlDatabase(const char *connStr, bool connect, int cmdPoolSize) :m_connection(), m_cmd(), m_connStr(connStr)
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
        catch (const odbc::otl_exception &err)
        {
            FormatExceptionMsg(err, m_lastError, "构造连接错误", true);
        }
    }
}

ODBCSqlDatabase::~ODBCSqlDatabase()
{
}

SqlDatabaseType ODBCSqlDatabase::Type()
{
    return SqlDatabaseType::ODBC;
}

bool ODBCSqlDatabase::Prepare(const char *sql, int bufferSize, bool stayInPool, bool storedPorcReturnDataset)
{
    bool ret = true;
    try
    {
        if (!m_connection.connected)
        {
            m_connection.rlogon(m_connStr.c_str(), false);
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
        m_cmd.open(bufferSize + 1, sql, m_connection, storedPorcReturnDataset ? otl_implicit_select : otl_explicit_select);//otl在buffer满时会自动flush，所以这里加1，防止最后一条记录放入buffer后自动执行
        m_cmdStayInPool = stayInPool;
        m_cmd.set_commit(false);
        m_cmd.set_flush(false);
    }
    catch (const odbc::otl_exception &err)
    {
        ret = false;
        FormatExceptionMsg(err, m_lastError, "准备SQL错误");
        HandleNetworkError(err);
    }
    return ret;
}

bool ODBCSqlDatabase::FlushBuffer()
{
    bool ret = true;
    try
    {
        m_cmd.flush();
    }
    catch (const odbc::otl_exception &err)
    {
        ret = false;
        FormatExceptionMsg(err, m_lastError, "执行SQL错误");
        HandleNetworkError(err);
    }
    return ret;
}

bool ODBCSqlDatabase::Commit()
{
    bool ret = true;
    try
    {
        m_connection.commit();
    }
    catch (const odbc::otl_exception &err)
    {
        ret = false;
        FormatExceptionMsg(err, m_lastError, "提交SQL错误");
        HandleNetworkError(err);
    }
    return ret;
}

bool ODBCSqlDatabase::Rollback()
{
    bool ret = true;
    try
    {
        m_connection.rollback();
    }
    catch (const odbc::otl_exception &err)
    {
        ret = false;
        FormatExceptionMsg(err, m_lastError, "回滚SQL错误");
        HandleNetworkError(err);
    }
    return ret;
}

int ODBCSqlDatabase::IsEOF()
{
    try
    {
        return m_cmd.eof();
    }
    catch (const odbc::otl_exception &err)
    {
        FormatExceptionMsg(err, m_lastError, "获取查询结束标志错误");
        HandleNetworkError(err);
    }
    return -1;
}

void ODBCSqlDatabase::HandleNetworkError(const odbc::otl_exception &err)
{
    if (IsNetworkError(err))
    {
        try
        {
            try
            {
                m_cmd.close(m_cmdStayInPool);
            }
            catch (...)
            {
            }
            m_connection.logoff();
        }
        catch (const odbc::otl_exception&)
        {
        }
    }
}

const std::string& ODBCSqlDatabase::LastError()
{
    return m_lastError;
}

ISqlDatabase& ODBCSqlDatabase::operator<<(const unsigned char* s)
{
    try
    {
        m_cmd << s;
        return *this;
    }
    catch (const odbc::otl_exception &err)
    {
        FormatExceptionMsg(err, m_lastError, "设置参数错误");
        HandleNetworkError(err);
        throw;
    }
}

ISqlDatabase& ODBCSqlDatabase::operator<<(const char c)
{
    try
    {
        m_cmd << c;
        return *this;
    }
    catch (const odbc::otl_exception &err)
    {
        FormatExceptionMsg(err, m_lastError, "设置参数错误");
        HandleNetworkError(err);
        throw;
    }
}

ISqlDatabase& ODBCSqlDatabase::operator<<(const char* s)
{
    try
    {
        m_cmd << s;
        return *this;
    }
    catch (const odbc::otl_exception &err)
    {
        FormatExceptionMsg(err, m_lastError, "设置参数错误");
        HandleNetworkError(err);
        throw;
    }
}

ISqlDatabase& ODBCSqlDatabase::operator<<(const otl_long_string& d)
{
    try
    {
        m_cmd << d;
        return *this;
    }
    catch (const odbc::otl_exception &err)
    {
        FormatExceptionMsg(err, m_lastError, "设置参数错误");
        HandleNetworkError(err);
        throw;
    }
}

ISqlDatabase& ODBCSqlDatabase::operator<<(const std::string& s)
{
    try
    {
        m_cmd << s;
        return *this;
    }
    catch (const odbc::otl_exception &err)
    {
        FormatExceptionMsg(err, m_lastError, "设置参数错误");
        HandleNetworkError(err);
        throw;
    }
}

ISqlDatabase& ODBCSqlDatabase::operator<<(const int n)
{
    try
    {
        m_cmd << n;
        return *this;
    }
    catch (const odbc::otl_exception &err)
    {
        FormatExceptionMsg(err, m_lastError, "设置参数错误");
        HandleNetworkError(err);
        throw;
    }
}

ISqlDatabase& ODBCSqlDatabase::operator<<(const unsigned int u)
{
    try
    {
        m_cmd << u;
        return *this;
    }
    catch (const odbc::otl_exception &err)
    {
        FormatExceptionMsg(err, m_lastError, "设置参数错误");
        HandleNetworkError(err);
        throw;
    }
}

ISqlDatabase& ODBCSqlDatabase::operator<<(const short sh)
{
    try
    {
        m_cmd << sh;
        return *this;
    }
    catch (const odbc::otl_exception &err)
    {
        FormatExceptionMsg(err, m_lastError, "设置参数错误");
        HandleNetworkError(err);
        throw;
    }
}

ISqlDatabase& ODBCSqlDatabase::operator<<(const long int l)
{
    try
    {
        m_cmd << l;
        return *this;
    }
    catch (const odbc::otl_exception &err)
    {
        FormatExceptionMsg(err, m_lastError, "设置参数错误");
        HandleNetworkError(err);
        throw;
    }
}

ISqlDatabase& ODBCSqlDatabase::operator<<(const us64 l)
{
    try
    {
        m_cmd << l;
        return *this;
    }
    catch (const odbc::otl_exception &err)
    {
        FormatExceptionMsg(err, m_lastError, "设置参数错误");
        HandleNetworkError(err);
        throw;
    }
}

ISqlDatabase& ODBCSqlDatabase::operator<<(const s64 l)
{
    try
    {
        m_cmd << l;
        return *this;
    }
    catch (const odbc::otl_exception &err)
    {
        FormatExceptionMsg(err, m_lastError, "设置参数错误");
        HandleNetworkError(err);
        throw;
    }
}

ISqlDatabase& ODBCSqlDatabase::operator<<(const float f)
{
    try
    {
        m_cmd << f;
        return *this;
    }
    catch (const odbc::otl_exception &err)
    {
        FormatExceptionMsg(err, m_lastError, "设置参数错误");
        HandleNetworkError(err);
        throw;
    }
}

ISqlDatabase& ODBCSqlDatabase::operator<<(const double d)
{
    try
    {
        m_cmd << d;
        return *this;
    }
    catch (const odbc::otl_exception &err)
    {
        FormatExceptionMsg(err, m_lastError, "设置参数错误");
        HandleNetworkError(err);
        throw;
    }
}

ISqlDatabase& ODBCSqlDatabase::operator<<(const otl_null &n)
{
    try
    {
        m_cmd << n;
        return *this;
    }
    catch (const odbc::otl_exception &err)
    {
        FormatExceptionMsg(err, m_lastError, "设置参数错误");
        HandleNetworkError(err);
        throw;
    }
}

ISqlDatabase& ODBCSqlDatabase::operator<<(const otl_datetime& dt)
{
    try
    {
        m_cmd << dt;
        return *this;
    }
    catch (const odbc::otl_exception &err)
    {
        FormatExceptionMsg(err, m_lastError, "设置参数错误");
        HandleNetworkError(err);
        throw;
    }
}

ISqlDatabase& ODBCSqlDatabase::operator >> (unsigned char* s)
{
    try
    {
        m_cmd >> s;
        return *this;
    }
    catch (const odbc::otl_exception &err)
    {
        FormatExceptionMsg(err, m_lastError, "读取查询结果错误");
        HandleNetworkError(err);
        throw;
    }
}

ISqlDatabase& ODBCSqlDatabase::operator >> (char& c)
{
    try
    {
        m_cmd >> c;
        return *this;
    }
    catch (const odbc::otl_exception &err)
    {
        FormatExceptionMsg(err, m_lastError, "读取查询结果错误");
        HandleNetworkError(err);
        throw;
    }
}

ISqlDatabase& ODBCSqlDatabase::operator >> (unsigned char& c)
{
    try
    {
        m_cmd >> c;
        return *this;
    }
    catch (const odbc::otl_exception &err)
    {
        FormatExceptionMsg(err, m_lastError, "读取查询结果错误");
        HandleNetworkError(err);
        throw;
    }
}

ISqlDatabase& ODBCSqlDatabase::operator >> (char* s)
{
    try
    {
        m_cmd >> s;
        return *this;
    }
    catch (const odbc::otl_exception &err)
    {
        FormatExceptionMsg(err, m_lastError, "读取查询结果错误");
        HandleNetworkError(err);
        throw;
    }
}

ISqlDatabase& ODBCSqlDatabase::operator >> (otl_long_string& s)
{
    try
    {
        m_cmd >> s;
        return *this;
    }
    catch (const odbc::otl_exception &err)
    {
        FormatExceptionMsg(err, m_lastError, "读取查询结果错误");
        HandleNetworkError(err);
        throw;
    }
}

ISqlDatabase& ODBCSqlDatabase::operator >> (std::string& s)
{
    try
    {
        m_cmd >> s;
        return *this;
    }
    catch (const odbc::otl_exception &err)
    {
        FormatExceptionMsg(err, m_lastError, "读取查询结果错误");
        HandleNetworkError(err);
        throw;
    }
}

ISqlDatabase& ODBCSqlDatabase::operator >> (int& n)
{
    try
    {
        m_cmd >> n;
        return *this;
    }
    catch (const odbc::otl_exception &err)
    {
        FormatExceptionMsg(err, m_lastError, "读取查询结果错误");
        HandleNetworkError(err);
        throw;
    }
}

ISqlDatabase& ODBCSqlDatabase::operator >> (unsigned int& u)
{
    try
    {
        m_cmd >> u;
        return *this;
    }
    catch (const odbc::otl_exception &err)
    {
        FormatExceptionMsg(err, m_lastError, "读取查询结果错误");
        HandleNetworkError(err);
        throw;
    }
}

ISqlDatabase& ODBCSqlDatabase::operator >> (short& sh)
{
    try
    {
        m_cmd >> sh;
        return *this;
    }
    catch (const odbc::otl_exception &err)
    {
        FormatExceptionMsg(err, m_lastError, "读取查询结果错误");
        HandleNetworkError(err);
        throw;
    }
}

ISqlDatabase& ODBCSqlDatabase::operator >> (long int& l)
{
    try
    {
        m_cmd >> l;
        return *this;
    }
    catch (const odbc::otl_exception &err)
    {
        FormatExceptionMsg(err, m_lastError, "读取查询结果错误");
        HandleNetworkError(err);
        throw;
    }
}

ISqlDatabase& ODBCSqlDatabase::operator >> (float& f)
{
    try
    {
        m_cmd >> f;
        return *this;
    }
    catch (const odbc::otl_exception &err)
    {
        FormatExceptionMsg(err, m_lastError, "读取查询结果错误");
        HandleNetworkError(err);
        throw;
    }
}

ISqlDatabase& ODBCSqlDatabase::operator >> (double& d)
{
    try
    {
        m_cmd >> d;
        return *this;
    }
    catch (const odbc::otl_exception &err)
    {
        FormatExceptionMsg(err, m_lastError, "读取查询结果错误");
        HandleNetworkError(err);
        throw;
    }
}

ISqlDatabase& ODBCSqlDatabase::operator >> (otl_datetime& dt)
{
    try
    {
        m_cmd >> dt;
        return *this;
    }
    catch (const odbc::otl_exception &err)
    {
        FormatExceptionMsg(err, m_lastError, "读取查询结果错误");
        HandleNetworkError(err);
        throw;
    }
}

void ODBCSqlDatabase::FormatExceptionMsg(const odbc::otl_exception &ex, std::string& msg, const char *context, bool onlyMsg)
{
    msg.clear();
    msg.append(context);
    msg.append("，错误信息：");
    msg.append(reinterpret_cast<const char*>(ex.msg));
    if (!onlyMsg)
    {
        msg.append("\r\nsql语句：");
        msg.append(ex.stm_text);
        msg.append("\r\nsql状态信息：");
        msg.append(reinterpret_cast<const char*>(ex.sqlstate));
    }
}

bool ODBCSqlDatabase::IsNetworkError(const odbc::otl_exception &ex)
{
    return (ex.sqlstate[0] == '0'&&ex.sqlstate[1] == '8') || (strcmp(reinterpret_cast<const char*>(ex.sqlstate), "40003") == 0)
        || (strcmp(reinterpret_cast<const char*>(ex.sqlstate), "HYT00") == 0) || (strcmp(reinterpret_cast<const char*>(ex.sqlstate), "HYT01") == 0)
#if defined(IS_WINDOWS)
        || (strcmp(reinterpret_cast<const char*>(ex.sqlstate), "01000") == 0)
#endif
        ;
}

#endif /* USE_ODBC_DATABASE_UTILS */