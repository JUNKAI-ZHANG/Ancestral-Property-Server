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

    switch (body->opt())
    {
    case LoginProto::LoginRequest_Operation_Login:
    {
        printf("%s %s try to login \n", body->username().c_str(), body->passwd().c_str());
        ret = QueryUser(body->username(), body->passwd());
        break;
    }
    case LoginProto::LoginRequest_Operation_Register:
    {
        if (IsExistUser(body->username()))
        {
            ret = 0;
            break;
        }
        printf("%s %s try to register \n", body->username().c_str(), body->passwd().c_str());
        ret = InsertUser(body->username(), body->passwd());
        break;
    }
    default:
    {
        break;
    }
    }

    // 异常错误
    if (ret == -1)
    {
        std::cerr << "Login/Register Error in handle" << std::endl;
        return;
    }

    std::string resp_msg;
    LoginProto::LoginResponse_Operation resp_opt;

    switch(body->opt())
    {
    case LoginProto::LoginRequest_Operation_Login:
    {
        if (ret == 1)
        {
            resp_msg = "Login Success!";
        }
        else if (ret == 0)
        {
            resp_msg = "Passwd Error";
        }
        resp_opt = LoginProto::LoginResponse_Operation_Login;
        break;
    }
    case LoginProto::LoginRequest_Operation_Register:
    {
        if (ret == 1)
        {
            resp_msg = "Register Success!";
        }
        else if (ret == 0)
        {
            resp_msg = "Register Failed, username is exist";
        }
        resp_opt = LoginProto::LoginResponse_Operation_Register;
        break;
    }
    default:
    {
        break;
    }
    }

    uint32_t token = 0;
    if (ret == 1)
    {
        token = Encryption::GenerateToken(body->username(), body->passwd());
        std::cout << "Token = " << token << std::endl;

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

    std::cout << "Connect to mysql success!" << std::endl;

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

    std::cout << "Connect to redis success!" << std::endl;

    return true;
}

bool DBServer::IsExistUser(const std::string username)
{
    if (mysql == nullptr)
    {
        std::cerr << "Connection with mysql was closed" << std::endl;
        return -1;
    }

    std::string sql = "SELECT * FROM user where username = '" + username + "'";
    std::cout << sql << std::endl;

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

    bool ret = false;

    MYSQL_ROW row;
    if ((row = mysql_fetch_row(result)))
    {
        ret = true;
    }
    return ret;
}

int DBServer::QueryUser(const std::string username, const std::string password)
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
    std::string sql = "SELECT * FROM user where username = '" + username + "' and passwd = '" + password + "'";

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

bool DBServer::InsertUser(const std::string username, const std::string password)
{
    if (mysql == nullptr)
    {
        std::cerr << "Connection with mysql was closed" << std::endl;
        return false;
    }
/*
* 手机号注册 Exception
    try
    {
        int num = std::stoi(username);
    }
    catch (const std::invalid_argument &e)
    {
        std::cerr << "username format error" << std::endl;
        return false;
    }
*/

    // 执行 SQL 插入用户信息
    std::string sql = "insert into user (username, passwd) VALUES (\'" + username + "\', \'" + password + "\')";

    if (mysql_query(mysql, sql.c_str()) != 0)
    {
        std::cerr << "Error: " << mysql_error(mysql) << std::endl;
        return false;
    }

    return true;
}

bool DBServer::ChangeUserMoney(const std::string username, int money, int &allMoney)
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
    std::string sql = "SELECT * FROM user_money where username = '" + username + "'";

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
    sql = "update user_money set money = " + std::to_string(currentMoney + money) + " where username = '" + username + "'";

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
        // printf("Usage: %s port\n", argv[0]);
        // return 1;
    }

    // int port = std::atoi(argv[1]);
    int port = 10811;

    DBServer dbServer;

    if (dbServer.ConnectToMysqlAndRedis())
    {
        printf("Start DB Server ing...\n");
        dbServer.BootServer(port);
    }

    return 0;
}