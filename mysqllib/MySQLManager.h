//
// Created by haowendeng on 2023/3/2.
//

#ifndef MYSQLMANAGER_H
#define MYSQLMANAGER_H

#include <iostream>
#include <vector>
#include <mysql/mysql.h>
#include "StatementFactory.h"

class MySQLManager {
public:
    MySQLManager() {
        m_connected = false;
        m_isTransaction = false;
        m_isTransactionError = false;
        m_statementFactory = nullptr;
    }

    void init();

    // 连接到服务器
    bool connect(const char *host, const char *username, const char *password, const char *schema, int port = 3306);

    // 开始事务
    bool begin_transaction();

    // 结束事务
    bool end_transaction();

    // 执行sql语句，返回查询结果集
    template<typename T>
    std::vector<T> executeQuery(const std::string &statement);

    // 获取查询结果的首个，如果结果集为空，返回null result
    template<typename T>
    T fetchOne(const std::string &statement);

    // 执行数据库操作语句，返回受影响的行数
    size_t executeUpdate(const std::string &statement);

    // 获得sql语句工厂
    StatementFactory *getStatementFactory();

    // 关闭连接
    void close_connection();

    ~MySQLManager() { close_connection(); }

private:
    bool m_isTransaction;
    bool m_isTransactionError;
    StatementFactory *m_statementFactory;
    MYSQL m_conn;
    bool m_connected;
};

#include "MySQLManager.tpp"

#endif //MYSQLMANAGER_H
