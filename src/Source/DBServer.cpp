#include "../Header/Server/DBServer.h"
#include "../Header/Tool/Encryption.hpp"

DBServer::DBServer()
{
    server_type = SERVER_TYPE::DATABASE;

    rng = std::mt19937(std::chrono::system_clock::now().time_since_epoch().count());
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
    FuncServer::OnMsgBodyAnalysised(msg, body, length, fd);

    switch (msg->head->m_packageType)
    {
    case BODYTYPE::LoginRequest:
    {
        HandleUserLogin(msg, fd);
        break;
    }
    case BODYTYPE::RegistRequest:
    {
        HandleUserRegister(msg, fd);
        break;
    }
    default:
    {
        break;
    }
    }
}

// ---------------------------------------------------------------------
// network msg handle
// ---------------------------------------------------------------------
void DBServer::HandleUserLogin(Message *msg, int fd)
{
    LoginProto::LoginRequest *body = reinterpret_cast<LoginProto::LoginRequest *>(msg->body->message);
    int ret = -1, userid = -1;

    std::cout << body->username() << ' ' << body->passwd() << " try to login" << std::endl;
    ret = QueryUser(body->username(), body->passwd(), userid);

    std::string resp_msg;

    if (ret == 1)
    {
        resp_msg = "Login Success!";
    }
    else if (ret == 0)
    {
        resp_msg = "Passwd Error";
    }
    else if (ret == -1)
    {
        // 异常错误
        std::cerr << "Login Error in handle(MySQL Layer)" << std::endl;
        return;
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

            reply = (redisReply *)redisCommand(redis, "EXPIRE %s %d", body->username().c_str(), rng() % 500 + 500); // 600s过期

            if (reply != nullptr)
            {
                // 释放 reply 对象
                freeReplyObject(reply);
            }
        }
    }
/*
    int userid = -1;
    if (ret == 1) 
    {
        userid = GetUserid(body->username());
    }
*/

    Message *message = NewLoginResponseMessage(ret, resp_msg, token, userid, msg->head->m_userid, body->username());

    if (message != nullptr)
    {
        SendMsg(message, fd);
        delete message;
    }
}

void DBServer::HandleUserRegister(Message *msg, int fd)
{
    LoginProto::RegistRequest *body = reinterpret_cast<LoginProto::RegistRequest *>(msg->body->message);
    int ret = -1;

    if (IsExistUser(body->username()))
    {
        ret = 0;
    }
    if (ret == -1)
    {
        std::cout << body->username() << ' ' << body->passwd() << " try to register" << std::endl;

        ret = InsertUser(body->username(), body->passwd());
    }

    std::string resp_msg = "";

    if (ret == 1)
    {
        resp_msg = "Register Success!";
    }
    else if (ret == 0)
    {
        resp_msg = "Register Failed, username is exist";
    }
    else if (ret == -1)
    {
        // 异常错误
        std::cerr << "Register Error in handle" << std::endl;
        return;
    }

    Message *message = NewRegisterResponseMessage(ret, resp_msg, msg->head->m_userid);

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
        return false;
    }

    std::string sql = "SELECT username FROM user where username = '" + SQL_Escape(username) + "'";

    if (mysql_query(mysql, sql.c_str()))
    {
        std::cerr << "Error: " << mysql_error(mysql) << std::endl;
        return false;
    }

    // 获取查询结果
    MYSQL_RES *result = mysql_store_result(mysql);
    if (result == nullptr)
    {
        std::cerr << "Error: " << mysql_error(mysql) << std::endl;
        return false;
    }

    bool ret = false;

    MYSQL_ROW row;
    if ((row = mysql_fetch_row(result)))
    {
        ret = true;
    }
    return ret;
}

int DBServer::QueryUser(const std::string username, const std::string password, int &userid)
{
    std::string key = "user" + username;

    redisReply *reply_pwd = (redisReply *)redisCommand(redis, "hget %s pwd", key.c_str());
    redisReply *reply_uid = (redisReply *)redisCommand(redis, "hget %s uid", key.c_str());

    if (reply_pwd != nullptr && reply_pwd->type == REDIS_REPLY_STRING && reply_uid != nullptr && reply_uid->type == REDIS_REPLY_INTEGER)
    {
        int ret = 0;

        if (password == reply_pwd->str)
        {
            ret = 1;
            userid = reply_uid->integer;
        }
        // 释放 reply 对象
        freeReplyObject(reply_pwd);
        freeReplyObject(reply_uid);
        return ret;
    }

    // 释放 reply 对象
    if (reply_pwd != nullptr) freeReplyObject(reply_pwd);
    if (reply_uid != nullptr) freeReplyObject(reply_uid);

    if (mysql == nullptr)
    {
        std::cerr << "Connection with mysql was closed" << std::endl;
        return -1;
    }

    // 执行 SQL 查询
    std::string sql = "SELECT username, passwd, userid FROM user where username = '" + SQL_Escape(username) + "'";

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
        if (row[1] == password)
        {
            ret = 1;
            userid = std::atoi(row[2]);
        }
    }

    // 释放资源
    mysql_free_result(result);

    // 缓存策略,加速下次查询
    if (ret == 1)
    {
        redisReply *reply = (redisReply *)redisCommand(redis, "hmset %s pwd \"%s\" uid %d", key.c_str(), password.c_str(), userid);
        if (reply != nullptr)
        {
            // 释放 reply 对象
            freeReplyObject(reply);

            reply = (redisReply *)redisCommand(redis, "EXPIRE %s %d", key.c_str(), rng() % 500 + 500); // ???s过期

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

    // 执行 SQL 插入用户信息
    std::string sql = "insert into user (username, passwd) VALUES ('" + SQL_Escape(username) + "', '" + SQL_Escape(password) + "')";

    if (mysql_query(mysql, sql.c_str()) != 0)
    {
        std::cerr << "Error: " << mysql_error(mysql) << std::endl;
        return false;
    }

    return true;
}

std::string DBServer::SQL_Escape(std::string sql)
{
    if (mysql == nullptr)
    {
        std::cerr << "Connection with mysql was closed" << std::endl;
        return false;
    }
    char result[sql.size() * 2];
    mysql_real_escape_string(mysql, result, sql.c_str(), sql.size());
    return result;
}

// ---------------------------------------------------------------------

int main(int argc, char **argv)
{
    const std::string ip = JSON.DATABASE_SERVER_IP;
    const int port = JSON.DATABASE_SERVER_PORT;

    DBServer dbServer;

    if (dbServer.ConnectToMysqlAndRedis())
    {
        std::cout << "Start DB Server ing..." << std::endl;
        dbServer.BootServer(ip, port, JSON.DATABASE_SERVER_NAME);
    }

    return 0;
}