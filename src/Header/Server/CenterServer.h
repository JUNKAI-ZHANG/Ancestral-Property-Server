#ifndef _CENTERSERVER_H
#define _CENTERSERVER_H

#include "ServerBase.h"

struct Server_Info
{
    std::string ip;

    int port;

    std::string name;

    SERVER_FREE_LEVEL level;

    SERVER_TYPE type;

    int people_count;

    int id;
};

/*
 * @brief
 * register server node info
 * such as : logicServer gateServer DBServer
 */
class CenterServer : public ServerBase
{
private:
    std::vector<Server_Info *> gateServerGroup;

    std::vector<Server_Info *> logicServerGroup;

    std::vector<Server_Info *> dbServerGroup;

    std::map<int, Server_Info *> MachineRecord;

private:
    void RegisterServer(Server_Info *machine);

    Server_Info FindAvailableServerInGroup(const std::vector<Server_Info*> &);

    Server_Info AssignServer(SERVER_TYPE in_type);

    void RemoveServerFromGroup(std::vector<Server_Info *> &, const Server_Info *);

    void RemoveServer(const Server_Info *);

private:
    void HandleServerInfo(Message *msg, int fd);

    void HandleUserInfo(Message *msg, int fd);

    void HandleGetRegionInfoRequest(Message *msg, int fd);

    void HandleJoinRegionRequest(Message *msg, int fd);

    void HandleServerConnChange(Message *msg, int fd);


protected:
    virtual void OnMsgBodyAnalysised(Message *msg, const uint8_t *body, uint32_t length, int fd);

public:
    explicit CenterServer();

    virtual ~CenterServer();

    virtual void CloseClientSocket(int fd);
};

#endif