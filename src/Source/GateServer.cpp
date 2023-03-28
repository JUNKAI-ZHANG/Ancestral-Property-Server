
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

void GateServer::OnMsgBodyAnalysised(Header head, const uint8_t *body, uint32_t length, int fd)
{
    bool parseRet = false;
    BODYTYPE type = head.type;

    Net::LoginData loginData;
    Net::LoginResponse loginResp;
    Net::ServerInfo server_info;
    Net::UserMoney userMoney;

    switch (type)
    {
    case BODYTYPE::LoginData:
        loginData = ProtoUtil::ParseBodyToLoginData(body, length, parseRet);

        // false 的情况已在util的函数里处理
        if (parseRet)
        {
            user_fd_record[loginData.username()] = fd;
            // 转发给db server处理
            SendMsg(BODYTYPE::LoginData, length, body, db_server_client);
        }

        break;
    case BODYTYPE::LoginResponse:
        loginResp = ProtoUtil::ParseBodyToLoginResponse(body, length, parseRet);

        // false 的情况已在util的函数里处理
        if (parseRet)
        {
            if (!user_fd_record.count(loginResp.userid()))
            {
                return;
            }

            int client_fd = user_fd_record[loginResp.userid()];
            SendMsg(BODYTYPE::LoginResponse, length, body, client_fd);

            if (loginResp.opt() == Net::LoginResponse_Operation_Register)
            {
                // 注册操作 移除临时record
                user_fd_record.erase(loginResp.userid());
            }
            else if (!loginResp.result())
            {
                // 登录失败 移除record
                user_fd_record.erase(loginResp.userid());
            }
        }

        break;
    case BODYTYPE::UserMoney:
        /* code */
        userMoney = ProtoUtil::ParseBodyToUserMoney(body, length, parseRet);

        // false 的情况已在util的函数里处理
        if (parseRet && fd != db_server_client)
        {
            // 转发给db server处理
            SendMsg(BODYTYPE::UserMoney, length, body, db_server_client);
        }
        else
        {
            if (!user_fd_record.count(userMoney.userid()))
            {
                return;
            }
            int client_fd = user_fd_record[userMoney.userid()];
            SendMsg(BODYTYPE::UserMoney, length, body, client_fd);
        }

        break;
    default:

        break;
    }

    FuncServer::OnMsgBodyAnalysised(head, body, length, fd);
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
