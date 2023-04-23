#ifndef _DBSERVER_H
#define _DBSERVER_H

#include <mysql/mysql.h>
#include <hiredis/async.h>
#include <hiredis/hiredis.h>

#include "FuncServer.h"

class DBServer : public FuncServer
{
private:
    MYSQL *mysql = nullptr;

    redisContext *redis = nullptr;

private:
    void HandleUserLogin(Message *msg, int fd);

    void HandleUserRegister(Message *msg, int fd);

    /* 尝试连接其他类型功能服务器 */
    virtual void TryToConnectAvailabeServer();

protected:
    /*
     * @brief
     * search from redis first,
     * if not exist then find in mysql
     *
     * @return
     * -1 means somewhere error
     *  0 means user not exist
     *  1 means find user success
     */
    int QueryUser(const std::string username, const std::string password);

    bool IsExistUser(const std::string username);

    bool InsertUser(const std::string username, const std::string password, const std::string userid);

    int GetUserid(std::string username);

    int GetRowCount(std::string tablename);

    bool ChangeUserMoney(const std::string username, int money, int&);

    virtual void OnMsgBodyAnalysised(Message *msg, const uint8_t *body, uint32_t length, int fd);

                                     
public:
    explicit DBServer();

    virtual ~DBServer();

    bool ConnectToMysqlAndRedis();
    
private:
    // mysql config - 我为了开发方便，你别偷我鸡
    const char *mysql_ip = "124.223.73.248";
    int mysql_port = 3306;
    const char *mysql_user = "root";
    const char *mysql_password = "Zjk20011019#";
    const char *db_name = "AncestralProperty";

    // redis config - 我为了开发方便，你别偷我鸡
    const char *redis_ip = "110.42.203.195";
    int redis_port = 6379;
    const char *redis_password = "Aa20010621++";

};

#endif