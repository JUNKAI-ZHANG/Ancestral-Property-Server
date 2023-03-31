#include "../Header/LogicServer.h"

LogicServer::LogicServer()
{
    server_type = SERVER_TYPE::LOGIC;
}

LogicServer::~LogicServer()
{
}

void LogicServer::CloseClientSocket(int fd)
{
    ServerBase::CloseClientSocket(fd);

    // 如果和db server断开连接 则尝试重连
    if (fd == db_server_client)
    {
        db_server_client = -1;
        TryToConnectAvailabeServer();
    }
}

void LogicServer::OnConnectToCenterServer()
{
    TryToConnectAvailabeServer();
}

void LogicServer::OnMsgBodyAnalysised(Message *msg, const uint8_t *body, uint32_t length, int fd)
{
    switch (msg->head->m_packageType)
    {
    default:
        break;
    }

    FuncServer::OnMsgBodyAnalysised(msg, body, length, fd);
}

void LogicServer::TryToConnectAvailabeServer()
{
    if (db_server_client == -1)
    {
        ApplyServerByType(SERVER_TYPE::DATABASE);
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

    LogicServer logicServer;

    printf("Start Logic Center Server ing...\n");
    logicServer.BootServer(port);

    return 0;
}
