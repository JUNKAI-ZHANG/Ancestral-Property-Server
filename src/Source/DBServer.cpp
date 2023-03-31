#include <chrono>

#include "../Header/DBServer.h"
#include "../Header/Encryption.hpp"

DBServer::DBServer()
{
    server_type = SERVER_TYPE::DATABASE;
}

DBServer::~DBServer()
{
    if (mysql != nullptr)
    {
        // 关闭 mysql
        mysql_close(mysql);
    }
    if (redis != nullptr)
    {
        // 关闭 Redis 连接
        redisFree(redis);
    }
}

void DBServer::TryToConnectAvailabeServer()
{
    // DBServer 无需连接其他功能服务器
}

void DBServer::OnMsgBodyAnalysised(Message *msg, const uint8_t *body, uint32_t length, int fd)
{
    ServerBase::OnMsgBodyAnalysised(msg, body, length, fd);

    switch (msg->head->m_packageType)
    {
    case BODYTYPE::LoginRequest:
    {
        LoginProto::LoginRequest *body = reinterpret_cast<LoginProto::LoginRequest *>(msg->body->message);
        printf("%s %s try to login/register \n", body->username().c_str(), body->passwd().c_str());
        HandleUserLogin(msg, fd);
        break;
    }
    }

    FuncServer::OnMsgBodyAnalysised(msg, body, length, fd);
}

// ---------------------------------------------------------------------
// network msg handle
// ---------------------------------------------------------------------
void DBServer::HandleUserLogin(Message *msg, int fd)
{
    LoginProto::LoginRequest *body = reinterpret_cast<LoginProto::LoginRequest *>(msg->body->message);
    int ret = -1;

    if (msg->head->m_packageType == LoginProto::LoginRequest_Operation_Login)
    {
        ret = QueryUser(body->username(), body->passwd());
    }
    else if (msg->head->m_packageType == LoginProto::LoginRequest_Operation_Register)
    {
        ret = InsertUser(body->username(), body->passwd());
    }

    // 异常错误
    if (ret == -1)
    {
        return;
    }

    std::string resp_msg;
    LoginProto::LoginResponse_Operation resp_opt;

    if (msg->head->m_packageType == LoginProto::LoginRequest_Operation_Login)
    {
        if (ret == 1)
        {
            resp_msg = "login success";
        }
        else if (ret == 0)
        {
            resp_msg = "passwd error";
        }
        resp_opt = LoginProto::LoginResponse_Operation_Login;
    }
    else if (msg->head->m_packageType == LoginProto::LoginRequest_Operation_Register)
    {
        if (ret == 1)
        {
            resp_msg = "register success";
        }
        else if (ret == 0)
        {
            resp_msg = "register failed";
        }
        resp_opt = LoginProto::LoginResponse_Operation_Register;
    }

    uint32_t token = 0;
    if (ret == 1)
    {
        token = Encryption::GenerateToken(body->username(), body->passwd());
        redisReply *reply = (redisReply *)redisCommand(redis, "set %s %u", body->username().c_str(), token);
        if (reply != nullptr)
        {
            // 释放 reply 对象
            freeReplyObject(reply);

            reply = (redisReply *)redisCommand(redis, "EXPIRE %s %d", body->username().c_str(), 600); // 600s过期

            if (reply != nullptr)
            {
                // 释放 reply 对象
                freeReplyObject(reply);
            }
        }
    }
    // std::cout << Encryption::GenerateToken(data.username(), data.passwd()) << std::endl;

    Message *message = NewLoginResponseMessage(ret, resp_msg, body->username(), token, resp_opt);

    if (message != nullptr)
    {
        SendMsg(message, fd);
        delete message;
    }
}

// ---------------------------------------------------------------------

// ---------------------------------------------------------------------
// sql operation
// ---------------------------------------------------------------------
bool DBServer::ConnectToMysqlAndRedis()
{
    // 连接 MySQL 数据库
    mysql = mysql_init(NULL);

    if (!mysql_real_connect(mysql, mysql_ip, mysql_user, mysql_password, db_name, mysql_port, NULL, 0))
    {
        std::cerr << "Failed to connect to database: Error: " << mysql_error(mysql) << std::endl;
        return false;
    }

    printf("Connect to mysql success\n");

    // 连接Redis服务器
    redis = redisConnect(redis_ip, redis_port);
    if (redis == NULL || redis->err)
    {
        std::cerr << "Failed to connect to Redis" << std::endl;
        return false;
    }

    // 密码验证
    redisReply *reply = (redisReply *)redisCommand(redis, "AUTH %s", redis_password);
    if (reply->type == REDIS_REPLY_ERROR)
    {
        printf("Error: %s\n", reply->str);
        freeReplyObject(reply);
        return false;
    }

    printf("Connect to redis success\n");

    return true;
}

int DBServer::QueryUser(std::string username, std::string password)
{
    std::string key = "user" + username;

    redisReply *reply = (redisReply *)redisCommand(redis, "get %s", key.c_str());

    if (reply != nullptr && reply->str != nullptr)
    {
        int ret = 0;

        if (std::string(reply->str) == password)
        {
            ret = 1;
        }
        // 释放 reply 对象
        freeReplyObject(reply);
        return ret;
    }

    if (mysql == nullptr)
    {
        std::cerr << "Connection with mysql was closed" << std::endl;
        return -1;
    }

    // 执行 SQL 查询
    std::string sql = "SELECT * FROM user where username = " + username + " and passwd = " + password;

    if (mysql_query(mysql, sql.c_str()))
    {
        std::cerr << "Error: " << mysql_error(mysql) << std::endl;
        return -1;
    }

    // 获取查询结果
    MYSQL_RES *result = mysql_store_result(mysql);
    if (result == nullptr)
    {
        std::cerr << "Error: " << mysql_error(mysql) << std::endl;
        return -1;
    }

    // 遍历查询结果
    // MYSQL_ROW row;
    // while ((row = mysql_fetch_row(result))) {
    //     std::cout << row[0] << ", " << row[1] << ", " << row[2] << std::endl;
    // }
    int ret = 0;

    MYSQL_ROW row;
    if ((row = mysql_fetch_row(result)))
    {
        ret = 1;
    }

    // 释放资源
    mysql_free_result(result);

    // 缓存策略,加速下次查询
    if (ret)
    {
        redisReply *reply = (redisReply *)redisCommand(redis, "set %s %s", key.c_str(), password.c_str());
        if (reply != nullptr)
        {
            // 释放 reply 对象
            freeReplyObject(reply);

            reply = (redisReply *)redisCommand(redis, "EXPIRE %s %d", key.c_str(), 600); // 600s过期

            if (reply != nullptr)
            {
                // 释放 reply 对象
                freeReplyObject(reply);
            }
        }
    }

    return ret;
}

bool DBServer::InsertUser(std::string username, std::string password)
{
    if (mysql == nullptr)
    {
        std::cerr << "Connection with mysql was closed" << std::endl;
        return false;
    }

    try
    {
        int num = std::stoi(username);
    }
    catch (const std::invalid_argument &e)
    {
        std::cerr << "username format error" << std::endl;
        return false;
    }

    // 执行 SQL 插入用户信息
    std::string sql = "insert into user (username, passwd) VALUES (\'" + username + "\', \'" + password + "\')";

    if (mysql_query(mysql, sql.c_str()) != 0)
    {
        std::cerr << "Error: " << mysql_error(mysql) << std::endl;
        return false;
    }

    // 执行 SQL 语句初始化玩家金币
    sql = "insert into user_money (username, money) VALUES (\'" + username + "\', \'" + "0" + "\')";

    if (mysql_query(mysql, sql.c_str()) != 0)
    {
        std::cerr << "Error: " << mysql_error(mysql) << std::endl;
        return false;
    }

    return true;
}

bool DBServer::ChangeUserMoney(std::string username, int money, int &allMoney)
{
    if (mysql == nullptr)
    {
        std::cerr << "Connection with mysql was closed" << std::endl;
        return false;
    }

    try
    {
        int num = std::stoi(username);
    }
    catch (const std::invalid_argument &e)
    {
        std::cerr << "username format error" << std::endl;
        return false;
    }

    // 执行 SQL 查询
    std::string sql = "SELECT * FROM user_money where username = " + username;

    if (mysql_query(mysql, sql.c_str()))
    {
        std::cerr << "Error: " << mysql_error(mysql) << std::endl;
        return false;
    }

    int currentMoney = 0;

    // 获取查询结果
    MYSQL_RES *result = mysql_store_result(mysql);
    if (result == nullptr)
    {
        std::cerr << "Error: " << mysql_error(mysql) << std::endl;
        return false;
    }

    MYSQL_ROW row;
    if ((row = mysql_fetch_row(result)))
    {
        currentMoney = std::stoi(row[1]);
    }
    else
    {
        return false;
    }

    // 如果是get opt 直接终止
    if (money == 0)
    {
        allMoney = currentMoney;
        return true;
    }

    // 执行 SQL 语句修改金币
    sql = "update user_money set money = " + std::to_string(currentMoney + money) + " where username = " + username;

    if (mysql_query(mysql, sql.c_str()) != 0)
    {
        std::cerr << "Error: " << mysql_error(mysql) << std::endl;
        return false;
    }

    allMoney = currentMoney + money;
    return true;
}

// ---------------------------------------------------------------------

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: %s port\n", argv[0]);
        return 1;
    }

    int port = std::atoi(argv[1]);

    DBServer dbServer;

    if (dbServer.ConnectToMysqlAndRedis())
    {
        printf("Start DB Server ing...\n");
        dbServer.BootServer(port);
        // {
        //     auto start_time = std::chrono::high_resolution_clock::now();
        //     int ret = dbServer.QueryUser("123", "123456");
        //     auto end_time = std::chrono::high_resolution_clock::now();
        //     auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count(); // 微秒
        //     std::cout << "ret:" << ret << " time(微秒):" << diff << std::endl;
        // }
        // {
        //     auto start_time = std::chrono::high_resolution_clock::now();
        //     int ret = dbServer.QueryUser("123", "123456");
        //     auto end_time = std::chrono::high_resolution_clock::now();
        //     auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count(); // 微秒
        //     std::cout << "ret:" << ret << " time(微秒):" << diff << std::endl;
        // }
    }

    return 0;
}