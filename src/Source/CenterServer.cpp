#include "../Header/CenterServer.h"

CenterServer::CenterServer()
{
    server_type = SERVER_TYPE::CENTER;
}

CenterServer::~CenterServer()
{

}

void CenterServer::CloseClientSocket(int fd)
{
    if (MachineRecord.count(fd))
    {
        // 从group中删除
        RemoveServer(MachineRecord[fd]);
        // 释放内存
        delete MachineRecord[fd];
        // 从map中删除
        MachineRecord.erase(fd);
    }

    ServerBase::CloseClientSocket(fd);
}

void CenterServer::RegisterServer(Server_Info *machine)
{
    SERVER_TYPE type = machine->type;

    if (type == SERVER_TYPE::LOGIC)
    {
        logicServerGroup.push_back(machine);
    }
    else if (type == SERVER_TYPE::GATE)
    {
        gateServerGroup.push_back(machine);
    }
    else if (type == SERVER_TYPE::DATABASE)
    {
        dbServerGroup.push_back(machine);
    }
}

Server_Info CenterServer::FindAvailableServerInGroup(const std::vector<Server_Info *> &group)
{
    Server_Info info;
    info.level = SERVER_FREE_LEVEL::DOWN;

    int index = -1;
    int minlevel = SERVER_FREE_LEVEL::DOWN;

    for (int i = 0; i < group.size(); i++)
    {
        if (group[i]->level < minlevel)
        {
            index = i;
            minlevel = group[i]->level;
        }
    }

    if (index != -1)
    {
        info = *group[index];
    }

    return info;
}

Server_Info CenterServer::AssignServer(SERVER_TYPE in_type)
{
    if (in_type == SERVER_TYPE::LOGIC)
    {
        return FindAvailableServerInGroup(logicServerGroup);
    }
    else if (in_type == SERVER_TYPE::GATE)
    {
        return FindAvailableServerInGroup(gateServerGroup);
    }
    else if (in_type == SERVER_TYPE::DATABASE)
    {
        return FindAvailableServerInGroup(dbServerGroup);
    }

    Server_Info info;
    info.level = SERVER_FREE_LEVEL::DOWN;
    return info;
}

void CenterServer::RemoveServerFromGroup(std::vector<Server_Info *> &group, const Server_Info *info)
{
    for (auto iter = group.begin(); iter != group.end(); iter++)
    {
        if ((*iter) == info)
        {
            group.erase(iter);
            break;
        }
    }
}

void CenterServer::RemoveServer(const Server_Info *info)
{
    if (info->type == SERVER_TYPE::LOGIC)
    {
        RemoveServerFromGroup(logicServerGroup, info);
    }
    else if (info->type == SERVER_TYPE::GATE)
    {
        RemoveServerFromGroup(gateServerGroup, info);
    }
    else if (info->type == SERVER_TYPE::DATABASE)
    {
        RemoveServerFromGroup(dbServerGroup, info);
    }

    printf("%s:%d is offline\n", info->ip.c_str(), info->port);
}

void CenterServer::OnMsgBodyAnalysised(Message *msg, const uint8_t *body, uint32_t length, int fd)
{
    ServerBase::OnMsgBodyAnalysised(msg, body, length, fd);

    switch (msg->head->m_packageType)
    {
    case BODYTYPE::ServerInfo:
    {
        HandleServerInfo(msg, fd);

        break;
    }
    default:
    {

        break;
    }
    }
}

void CenterServer::HandleServerInfo(Message *msg, int fd)
{
    // 千万不要delete body;
    ServerProto::ServerInfo *body = reinterpret_cast<ServerProto::ServerInfo *>(msg->body->message);

    switch (body->opt())
    {
    case ServerProto::ServerInfo_Operation_Register:
    {
        // 如果记录不存在 注册一条记录
        if (!MachineRecord.count(fd))
        {
            MachineRecord[fd] = new Server_Info();
            MachineRecord[fd]->ip = body->ip();
            MachineRecord[fd]->port = body->port();
            MachineRecord[fd]->type = static_cast<SERVER_TYPE>(body->server_type());

            RegisterServer(MachineRecord[fd]);
            std::cout << "Register server success" << std::endl;
        }

        // 更新服务器状态
        MachineRecord[fd]->level = static_cast<SERVER_FREE_LEVEL>(body->server_free_level());
        break;
    }

    case ServerProto::ServerInfo_Operation_RequstAssgin:
    {
        Server_Info machine = AssignServer(static_cast<SERVER_TYPE>(body->server_type()));

        if (machine.level == SERVER_FREE_LEVEL::DOWN)
        {
            std::cout << "Assgin server failed, machine is not enough" << std::endl;
        }
        else
        {
            std::cout << "Assgin server success" << std::endl;
        }
        Message *message = NewServerInfoMessage(machine.ip, machine.port, machine.type, ServerProto::ServerInfo_Operation_Connect, machine.level);
        if (message != nullptr)
        {
            SendMsg(message, fd);
            delete message;
        }
        break;
    }
    default :
    {
        std::cout << "Type = " << msg->head->m_packageType << std::endl;
        std::cout << "Error ServerInfo Type" << std::endl;
        break;
    }
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: %s port\n", argv[0]);
        return 1;
    }

    int port = std::atoi(argv[1]);

    CenterServer centerServer;

    printf("Start Center Server ing...\n");
    centerServer.BootServer(port);

    return 0;
}