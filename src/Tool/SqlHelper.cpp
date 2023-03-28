#ifndef _SQLHELPER_H
#define _SQLHELPER_H

#include <iostream>
#include <map>
#include <mysql/mysql.h>
#include <hiredis/hiredis.h>
#include <hiredis/async.h>

// 操作mysql的实例类
class MysqlInst
{
private:
    // mysql config
    const char *mysql_ip = "110.42.203.195";
    int mysql_port = 3306;
    const char *mysql_user = "root";
    const char *mysql_password = "Aa20010621++";
    const char *db_name = "gameDemo";

private:
    MYSQL *mysql = nullptr;

public:
    explicit MysqlInst()
    {
        // 连接 MySQL 数据库
        mysql = mysql_init(NULL);

        if (!mysql_real_connect(mysql, mysql_ip, mysql_user, mysql_password, db_name, mysql_port, NULL, 0))
        {
            std::cerr << "Failed to connect to database: Error: " << mysql_error(mysql) << std::endl;
            return;
        }

        printf("Connect to mysql success\n");
    }

    ~MysqlInst()
    {
        if (mysql != nullptr)
        {
            // 关闭 mysql
            mysql_close(mysql);
        }
    }

    bool Insert(std::string tablename, const std::map<std::string, std::string> &objs)
    {
        // 执行 SQL 插入用户信息
        // std::string sql = "insert into user (username, passwd) VALUES (\'" + username + "\', \'" + password + "\')";

        std::string pre1 = ", ";
        tablename = "user";

        bool first = false;
        std::string sql = "insert into " + tablename + " (";
        for (const auto &it : objs)
        {
            if (!first)
            {
                first = true;
                sql = sql + it.first;
            }
            else
            {
                sql = sql + pre1 + it.first;
            }
        }
        sql = sql + ") VALUES (";
        first = false;
        for (const auto &it : objs)
        {
            std::string tmp = "\'" + it.second + "\'";
            if (!first)
            {
                first = true;
                sql = sql + tmp;
            }
            else
            {
                sql = sql + pre1 + tmp;
            }
        }
        sql = sql + ")";

        if (mysql_query(mysql, sql.c_str()) != 0)
        {
            std::cerr << "Error: " << mysql_error(mysql) << std::endl;
            return false;
        }

        return true;
    }
};

// 操作redis的实例类
class RedisInst
{
private:
    // redis config
    const char *redis_ip = "110.42.203.195";
    int redis_port = 6379;
    const char *redis_password = "Aa20010621++";

private:
    redisContext *redis = nullptr;

public:
    explicit RedisInst()
    {
        // 连接Redis服务器
        redis = redisConnect(redis_ip, redis_port);
        if (redis == NULL || redis->err)
        {
            std::cerr << "Failed to connect to Redis" << std::endl;
            return;
        }

        // 密码验证
        redisReply *reply = (redisReply *)redisCommand(redis, "AUTH %s", redis_password);
        if (reply->type == REDIS_REPLY_ERROR)
        {
            printf("Error: %s\n", reply->str);
            freeReplyObject(reply);
            return;
        }

        printf("Connect to redis success\n");
    }

    ~RedisInst()
    {
        if (redis != nullptr)
        {
            // 关闭 Redis 连接
            redisFree(redis);
        }
    }

    // TTL 为自动失效时间
    void Insert(std::string key, std::string value, int ttl = -1)
    {
        redisReply *reply = (redisReply *)redisCommand(redis, "set %s %s", key.c_str(), value.c_str());
        if (reply != nullptr)
        {
            // 释放 reply 对象
            freeReplyObject(reply);

            reply = (redisReply *)redisCommand(redis, "EXPIRE %s %d", key.c_str(), ttl); // ttl秒后过期

            if (reply != nullptr)
            {
                // 释放 reply 对象
                freeReplyObject(reply);
            }
        }
    }
};

int main()
{
    MysqlInst m;
    std::map<std::string, std::string> params;
    params.insert({"username", "999"});
    params.insert({"passwd", "999"});
    m.Insert("user", params);
    return 0;
}
#endif