//
// Created by haowendeng on 2023/3/2.
//

#ifndef MYSQLMANAGER_CPP
#define MYSQLMANAGER_CPP

#include "QueryResult.h"

#define RETURN_SQL_ERR(retval...) do{ \
    if(m_isTransaction) m_isTransactionError = true; \
    fprintf(stderr, "[Error] In %s (line %d):\n  %s:\n  %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, mysql_error(&m_conn)); \
    return retval;\
}while(0)

void MySQLManager::init() {
    if(m_connected) mysql_close(&m_conn);
    m_connected = false;
    m_isTransaction = false;
    m_isTransactionError = false;
    delete m_statementFactory;
    m_statementFactory = nullptr;
}

bool MySQLManager::connect(const char *host, const char *username, const char *password, const char *schema, int port) {
    if (m_connected) return false;
    mysql_init(&m_conn);
    mysql_options(&m_conn, MYSQL_SET_CHARSET_NAME, "utf8mb4");
    mysql_options(&m_conn, MYSQL_OPT_RECONNECT, "1");
    if (mysql_real_connect(&m_conn, host, username, password, schema, port, nullptr, 0) == nullptr)
        RETURN_SQL_ERR(false);
    m_statementFactory = new StatementFactory(&m_conn);
    m_connected = true;
    return true;
}

bool MySQLManager::begin_transaction() {
    m_isTransaction = true;
    m_isTransactionError = false;
    return !mysql_autocommit(&m_conn, false);
}

bool MySQLManager::end_transaction() {
    m_isTransaction = false;
    if (m_isTransactionError) {
        m_isTransactionError = false;
        return mysql_rollback(&m_conn) && !mysql_autocommit(&m_conn, true);
    }
    return !mysql_commit(&m_conn) && !mysql_autocommit(&m_conn, true);
}

size_t MySQLManager::executeUpdate(const std::string &statement) {
    if (m_isTransaction && m_isTransactionError) return 0;
    if (mysql_ping(&m_conn)) RETURN_SQL_ERR(0);
    if (mysql_query(&m_conn, statement.c_str())) RETURN_SQL_ERR(0);
    return mysql_affected_rows(&m_conn);
}

StatementFactory *MySQLManager::getStatementFactory() {
    if (m_connected) return m_statementFactory;
    else return nullptr;
}

void MySQLManager::close_connection() {
    init();
}

template<typename T>
T MySQLManager::fetchOne(const std::string &statement) {
    static_assert(std::is_base_of<QueryResult, T>::value, "type T should inherit from QueryResult");
    T res = QueryResult::createNullObject<T>();
    if (m_isTransaction && m_isTransactionError) return res;
    if (mysql_ping(&m_conn)) RETURN_SQL_ERR(res);
    if (mysql_query(&m_conn, statement.c_str())) RETURN_SQL_ERR(res);
    MYSQL_RES *result;
    result = mysql_use_result(&m_conn);
    if (!result) RETURN_SQL_ERR(res);
    MYSQL_ROW row;
    size_t column_count = mysql_num_fields(result);
    // 如果不为空就拿第一个
    if ((row = mysql_fetch_row(result))) {
        T object;
        size_t *lengths = mysql_fetch_lengths(result);
        object.decode(row, lengths, column_count);
        res = object;
    }
    mysql_free_result(result);
    return res;
}

template<typename T>
std::vector<T> MySQLManager::executeQuery(const std::string &statement) {
    static_assert(std::is_base_of<QueryResult, T>::value, "type T should inherit from QueryResult");
    std::vector<T> res;
    if (m_isTransaction && m_isTransactionError) return res;
    if (mysql_ping(&m_conn)) RETURN_SQL_ERR(res);
    if (mysql_query(&m_conn, statement.c_str())) RETURN_SQL_ERR(res);
    MYSQL_RES *result;
    result = mysql_store_result(&m_conn);
    if (!result) RETURN_SQL_ERR(res);
    MYSQL_ROW row;
    size_t column_count = mysql_num_fields(result);
    while ((row = mysql_fetch_row(result))) {
        T object;
        size_t *lengths = mysql_fetch_lengths(result);
        object.decode(row, lengths, column_count);
        res.push_back(object);
    }
    mysql_free_result(result);
    return res;
}

#undef RETURN_SQL_ERR

#endif // MYSQLMANAGER_CPP