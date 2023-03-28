#include "../Header/FuncServer.h"

FuncServer::FuncServer()
{
    // 无属性服务器
    server_type = SERVER_TYPE::NONE;
}

FuncServer::~FuncServer()
{
    center_connect_timer.stop();
}

void FuncServer::CloseServer()
{
    ServerBase::CloseServer();

    if (center_server_client > 0)
    {
        close(center_server_client);
    }

    if (logic_server_client > 0)
    {
        close(logic_server_client);
    }

    if (db_server_client > 0)
    {
        close(db_server_client);
    }
}

void FuncServer::ApplyServerByType(SERVER_TYPE InType)
{
    uint8_t *msg = nullptr;
    int msg_length = 0;

    msg = ProtoUtil::SerializeServerInfoToArray("",
                                                0,
                                                InType,
                                                Net::ServerInfo_Operation_RequstAssgin,
                                                SERVER_FREE_LEVEL::FREE,
                                                msg_length);

    if (msg == nullptr)
        return;

    SendMsg(BODYTYPE::ServerInfo, msg_length, msg, center_server_client);
    delete msg;
}

void FuncServer::OnConnectToCenterServer()
{
}

void FuncServer::SendSelfInfoToCenter()
{
    uint8_t *msg = nullptr;
    int msg_length = 0;

    msg = ProtoUtil::SerializeServerInfoToArray("127.0.0.1",
                                                this->listen_port,
                                                server_type,
                                                Net::ServerInfo_Operation_Register,
                                                SERVER_FREE_LEVEL::FREE,
                                                msg_length);

    if (msg == nullptr)
        return;

    // 如果发送失败 意味和center server断开连接 尝试重连
    if (!SendMsg(BODYTYPE::ServerInfo, msg_length, msg, center_server_client))
    {
        // 连接中心服务器
        if (!ConnectToOtherServer(center_ip, center_port, center_server_client))
        {
            std::cerr << "Failed to connect center server, boot it first\n";
            return;
        }

        if (conn_epoll->AddEventToEpoll(center_server_client) == -1)
        {
            close(center_server_client);
            return;
        }

        this->connections[center_server_client] = new RingBuffer();
        printf("connect to center server success \n");

        // 连接成功后立即发送一次自身状态
        this->SendSelfInfoToCenter();
        
        OnConnectToCenterServer();
    }

    delete msg;
}

bool FuncServer::OnListenerStart()
{
    // 定时发送自身信息给center server
    this->SendSelfInfoToCenter();

    center_connect_timer.start(5000, &FuncServer::SendSelfInfoToCenter, this);

    return true;
}

void FuncServer::OnMsgBodyAnalysised(Header head, const uint8_t *body, uint32_t length, int fd)
{
    bool parseRet = false;
    BODYTYPE type = head.type;
    
    Net::ServerInfo server_info;
    switch (type)
    {
    case BODYTYPE::ServerInfo:
        /* code */
        server_info = ProtoUtil::ParseBodyToServerInfo(body, length, parseRet);

        // false 的情况已在util的函数里处理
        if (parseRet)
        {
            HandleServerInfo(server_info, fd);
        }

        break;
    default:
        break;
    }
}

void FuncServer::HandleServerInfo(Net::ServerInfo &data, int fd)
{
    if (data.opt() == Net::ServerInfo_Operation_Connect)
    {
        if (static_cast<SERVER_TYPE>(data.server_type()) == SERVER_TYPE::LOGIC)
        {
            // 如果连接已存在 return
            if (logic_server_client != -1)
            {
                return;
            }

            if (!ConnectToOtherServer(data.ip(), data.port(), logic_server_client))
            {
                std::cerr << "Failed to connect Logic server, boot it first\n";
                return;
            }

            if (conn_epoll->AddEventToEpoll(logic_server_client) == -1)
            {
                close(logic_server_client);
                return;
            }

            this->connections[logic_server_client] = new RingBuffer();
            printf("connect to logic server success \n");
        }

        else if (static_cast<SERVER_TYPE>(data.server_type()) == SERVER_TYPE::DATABASE)
        {
            // 如果连接已存在 return
            if (db_server_client != -1)
            {
                return;
            }

            if (!ConnectToOtherServer(data.ip(), data.port(), db_server_client))
            {
                std::cerr << "Failed to connect database server, boot it first\n";
                return;
            }

            if (conn_epoll->AddEventToEpoll(db_server_client) == -1)
            {
                close(db_server_client);
                return;
            }

            this->connections[db_server_client] = new RingBuffer();
            printf("connect to database server success \n");
        }

        Timer timer;
        // 1s 后再次尝试重连
        timer.startOnce(1000, &FuncServer::TryToConnectAvailabeServer, this);
    }
}
