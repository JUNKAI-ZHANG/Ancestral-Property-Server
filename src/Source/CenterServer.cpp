#include "../Header/Server/CenterServer.h"

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
        machine->id = logicServerGroup.size();
        logicServerGroup.push_back(machine);
    }
    else if (type == SERVER_TYPE::GATE)
    {
        machine->id = gateServerGroup.size();
        gateServerGroup.push_back(machine);
    }
    else if (type == SERVER_TYPE::DATABASE)
    {
        machine->id = dbServerGroup.size();
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

    std::cout << info->ip << ":" << info->port << " is offline" << std::endl;
}

void CenterServer::OnMsgBodyAnalysised(Message *msg, const uint8_t *body, uint32_t length, int fd)
{
    ServerBase::OnMsgBodyAnalysised(msg, body, length, fd);

    // Center Server 只会收到服务器的消息
    switch (msg->head->m_packageType)
    {
    case BODYTYPE::ServerInfo:
    {
        HandleServerInfo(msg, fd);
        break;
    }
    case BODYTYPE::UserInfo:
    {
        HandleUserInfo(msg, fd);
        break;
    }
    case BODYTYPE::GetRegionInfoRequest:
    {
        HandleGetRegionInfoRequest(msg, fd);
        break;
    }
    case BODYTYPE::JoinRegionRequest:
    {
        HandleJoinRegionRequest(msg, fd);
        break;
    }
    case BODYTYPE::ServerConnChange:
    {
        HandleServerConnChange(msg, fd);
    }
    default:
    {

        break;
    }
    }
}

ServerProto::SERVER_FREE_LEVEL CheckFreeLevel(Server_Info *info)
{
    int count = info->people_count;
    if (count < JSON.SERVER_FREE_COUNT)   return ServerProto::SERVER_FREE_LEVEL::FREE;
    if (count < JSON.SERVER_COMMON_COUNT) return ServerProto::SERVER_FREE_LEVEL::COMMON;
    return ServerProto::SERVER_FREE_LEVEL::BUSY;
}

void CenterServer::HandleServerInfo(Message *msg, int fd)
{
    // 千万不要delete body;
    auto body = dynamic_cast<ServerProto::ServerInfo *>(msg->body->message);
    if (body == nullptr)
        return;

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
            std::cout << "Register server success!" << std::endl;
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
            std::cout << "Assgin server failed, machine is not enough..." << std::endl;
        }
        else
        {
            std::cout << "Assgin server success! " + machine.ip + ":" + to_string(machine.port) << std::endl;
        }
        Message *message = NewServerInfoMessage(machine.ip, machine.port, machine.type, ServerProto::ServerInfo_Operation_Connect, machine.level);
        if (message != nullptr)
        {
            SendMsg(message, fd);
            delete message;
        }
        break;
    }
    default:
    {
        std::cerr << "Error ServerInfo Type = " << msg->head->m_packageType << std::endl;
        break;
    }
    }
}

void CenterServer::HandleUserInfo(Message *msg, int fd)
{
    // 暂时没有功能需要实现
}

void CenterServer::HandleGetRegionInfoRequest(Message *msg, int fd)
{
    Message *message = new Message();
    message->body = new MessageBody();
    ServerProto::GetRegionInfoResponse *body = new ServerProto::GetRegionInfoResponse();
    ServerProto::RegionInfo *info_back = new ServerProto::RegionInfo();
    for (Server_Info * const info : gateServerGroup)
    {
        info_back = body->add_infos();
        info_back->set_id(info->id);
        info_back->set_level(CheckFreeLevel(info));
        info_back->set_people_count(std::max(0, info->people_count));
    }
    message->body->message = body;

    message->head = new MessageHead(message->length(), (int)BODYTYPE::GetRegionInfoResponse, 0);
    SendMsg(message, fd);
    delete message;
}

void CenterServer::HandleJoinRegionRequest(Message *msg, int fd)
{
    ServerProto::JoinRegionRequest *request = dynamic_cast<ServerProto::JoinRegionRequest *>(msg->body->message);

    Message *message = new Message();
    message->body = new MessageBody();
    ServerProto::JoinRegionResponse *body = new ServerProto::JoinRegionResponse();

    body->set_ret(false);
    body->set_ip("");
    body->set_port(0);

    int id = request->id();
    
    if (0 <= id && id < gateServerGroup.size())
    {
        body->set_ret(true);
        body->set_ip("124.223.73.248");
        body->set_port(gateServerGroup[id]->port);
    }

    message->body->message = body;

    message->head = new MessageHead(message->length(), (int)BODYTYPE::JoinRegionResponse, 0);
    SendMsg(message, fd);
    delete message;
}

void CenterServer::HandleServerConnChange(Message *msg, int fd)
{
    ServerProto::ServerConnChange *request = dynamic_cast<ServerProto::ServerConnChange *>(msg->body->message);

    ServerProto::SERVER_TYPE type = request->type();
    std::string ip = request->ip();
    int port = request->port();
    int count = request->change();
    if (type == ServerProto::SERVER_TYPE::GATE)  for (auto & server : gateServerGroup)  if (ip == server->ip && port == server->port) server->people_count = count;
    if (type == ServerProto::SERVER_TYPE::LOGIC) for (auto & server : logicServerGroup) if (ip == server->ip && port == server->port) server->people_count = count;
    if (type == ServerProto::SERVER_TYPE::DATABASE) for (auto & server : dbServerGroup) if (ip == server->ip && port == server->port) server->people_count = count;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        // printf("Usage: %s port\n", argv[0]);
        // return 1;
    }

    // int port = std::atoi(argv[1]);
    int port = JSON.CENTER_SERVER_PORT;

    CenterServer centerServer;

    std::cout << "Start Center Server ing..." << std::endl;

    centerServer.BootServer(port);

    return 0;
}