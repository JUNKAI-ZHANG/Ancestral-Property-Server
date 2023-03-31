#include "../Header/GateServer.h"

GateServer::GateServer()
{
    server_type = SERVER_TYPE::GATE;
}

GateServer::~GateServer()
{
}

void GateServer::CloseClientSocket(int fd)
{
    ServerBase::CloseClientSocket(fd);

    // 如果和logic server断开连接 告知所有客户端并则尝试重连
    if (fd == logic_server_client)
    {
        // notice clients
        DisconnectAllClients();

        // retry
        logic_server_client = -1;
        TryToConnectAvailabeServer();
    }

    // 如果和db server断开连接 则尝试重连
    if (fd == db_server_client)
    {
        db_server_client = -1;
        TryToConnectAvailabeServer();
    }
}

void GateServer::OnConnectToCenterServer()
{
    TryToConnectAvailabeServer();
}

void GateServer::TryToConnectAvailabeServer()
{
    if (logic_server_client == -1)
    {
        ApplyServerByType(SERVER_TYPE::LOGIC);
    }

    if (db_server_client == -1)
    {
        ApplyServerByType(SERVER_TYPE::DATABASE);
    }
}

void GateServer::DisconnectAllClients()
{
    for (auto it = connections.begin(); it != connections.end();)
    {
        if (it->first == center_server_client)
        {
            ++it;
        }
        else if (it->first == logic_server_client)
        {
            ++it;
        }
        else if (it->first == db_server_client)
        {
            ++it;
        }
        else
        {
            auto cur = it;
            ++it;
            CloseClientSocket(cur->first);
        }
    }
}

void GateServer::OnMsgBodyAnalysised(Message *msg, const uint8_t *body, uint32_t length, int fd)
{
    ServerBase::OnMsgBodyAnalysised(msg, body, length, fd);

    switch (msg->head->m_packageType)
    {
    case BODYTYPE::LoginRequest:
    {
        LoginProto::LoginRequest *body1 = reinterpret_cast<LoginProto::LoginRequest *>(msg->body->message);
        user_fd_record[body1->username()] = fd;
        // 转发给db server处理
        SendMsg(msg, db_server_client);
        break;
    }
    case BODYTYPE::LoginResponse:
    {
        LoginProto::LoginResponse *body2 = reinterpret_cast<LoginProto::LoginResponse *>(msg->body->message);
        if (!user_fd_record.count(body2->userid()))
        {
            return;
        }

        int client_fd = user_fd_record[body2->userid()];
        SendMsg(msg, client_fd);

        if (body2->opt() == LoginProto::LoginResponse_Operation_Register)
        {
            // 注册操作 移除临时record
            user_fd_record.erase(body2->userid());
        }
        else if (!(body2->result()))
        {
            // 登录失败 移除record
            user_fd_record.erase(body2->userid());
        }
        break;
    }
    }

    FuncServer::OnMsgBodyAnalysised(msg, body, length, fd);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: %s port\n", argv[0]);
        return 1;
    }

    int port = std::atoi(argv[1]);

    GateServer gateServer;

    printf("Start Gate Center Server ing...\n");
    gateServer.BootServer(port);

    return 0;
}
