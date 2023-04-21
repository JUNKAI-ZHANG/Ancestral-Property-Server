#include "../Header/FuncServer.h"

FuncServer::FuncServer()
{
    // 无属性服务器
    server_type = SERVER_TYPE::NONE;

    Timer *timer = new Timer(TRY_CONNECT_SERVER_TIME, CallbackType::FuncServer_TryToConnectAvailabeServer, std::bind(&FuncServer::TryToConnectAvailabeServer, this));
    timer->SetOnce();

    m_callfuncList.push_back(timer);
}

FuncServer::~FuncServer()
{
}

void FuncServer::CloseServer()
{
    ServerBase::CloseServer();

    if (center_server_client > 0)
    {
        close(center_server_client);
        conns_count--;
        if (server_type != SERVER_TYPE::CENTER)
            SendToCenterServerConnChange(TransformType(server_type), listen_ip, listen_port, conns_count);
    }

    if (logic_server_client > 0)
    {
        close(logic_server_client);
        conns_count--;
        if (server_type != SERVER_TYPE::CENTER)
            SendToCenterServerConnChange(TransformType(server_type), listen_ip, listen_port, conns_count);
    }

    if (db_server_client > 0)
    {
        close(db_server_client);
        conns_count--;
        if (server_type != SERVER_TYPE::CENTER)
            SendToCenterServerConnChange(TransformType(server_type), listen_ip, listen_port, conns_count);
    }
}

void FuncServer::ApplyServerByType(SERVER_TYPE InType)
{
    Message *msg = NewServerInfoMessage("", 0, InType, ServerProto::ServerInfo_Operation_RequstAssgin, SERVER_FREE_LEVEL::FREE);

    SendMsg(msg, center_server_client);

    delete msg;
}

void FuncServer::OnConnectToCenterServer()
{
}

SERVER_FREE_LEVEL FuncServer::DynamicCalcServerFreeLevel(int conns)
{
    if (conns == 0)
        return SERVER_FREE_LEVEL::FREE;
    if (conns == 1)
        return SERVER_FREE_LEVEL::COMMON;
    if (conns >= 2)
        return SERVER_FREE_LEVEL::BUSY;
}

void FuncServer::SendSelfInfoToCenter()
{
    Message *msg = NewServerInfoMessage(LOCAL_IP, this->listen_port, server_type, ServerProto::ServerInfo_Operation_Register, FuncServer::DynamicCalcServerFreeLevel(conns_count));

    if (msg == nullptr)
    {
        return;
    }

    // 如果发送失败 意味和center server断开连接 尝试重连
    if (!SendMsg(msg, center_server_client))
    {
        // 连接中心服务器
        if (!ConnectToOtherServer(LOCAL_IP, CENTER_SERVER_PORT, center_server_client))
        {
            std::cerr << "Failed to connect center server, boot it first" << std::endl;
            return;
        }

        if (listen_epoll->AddEventToEpoll(center_server_client) == -1)
        {
            close(center_server_client);
            return;
        }

        this->connections[center_server_client] = new RingBuffer();
        std::cout << "Connect to center server success! " + (std::string)LOCAL_IP + ":" + to_string(CENTER_SERVER_PORT) << std::endl;

        // 连接成功后立即发送一次自身状态
        this->SendSelfInfoToCenter();

        OnConnectToCenterServer();
    }

    delete msg;
}

void FuncServer::Update()
{
    // 这里应该也没什么功能需要实现，所以继续递归子类实现
}

void FuncServer::OnMsgBodyAnalysised(Message *msg, const uint8_t *body, uint32_t length, int fd)
{
    ServerBase::OnMsgBodyAnalysised(msg, body, length, fd);
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
    default:
    {
        // std::cout << "FuncServer : Not found package(" << msg->head->m_packageType << ")type..., will goto FuncServer!" << std::endl;
        break;
    }
    }
}

void FuncServer::HandleServerInfo(Message *msg, int fd)
{
    ServerProto::ServerInfo *body = reinterpret_cast<ServerProto::ServerInfo *>(msg->body->message);
    if (body->opt() == ServerProto::ServerInfo_Operation_Connect)
    {
        if (static_cast<SERVER_TYPE>(body->server_type()) == SERVER_TYPE::LOGIC)
        {
            // 如果连接已存在 return
            if (logic_server_client != -1)
            {
                return;
            }

            if (!ConnectToOtherServer(body->ip(), body->port(), logic_server_client))
            {
                std::cerr << "Failed to connect Logic server, boot it first" << std::endl;
                return;
            }

            if (listen_epoll->AddEventToEpoll(logic_server_client) == -1)
            {
                close(logic_server_client);
                return;
            }

            this->connections[logic_server_client] = new RingBuffer();
            std::cout << "Connect to logic server success! " + body->ip() + ":" + to_string(body->port()) << std::endl;;
        }

        else if (static_cast<SERVER_TYPE>(body->server_type()) == SERVER_TYPE::DATABASE)
        {
            // 如果连接已存在 return
            if (db_server_client != -1)
            {
                return;
            }

            if (!ConnectToOtherServer(body->ip(), body->port(), db_server_client))
            {
                std::cerr << "Failed to connect database server, boot it first" << std::endl;
                return;
            }

            if (listen_epoll->AddEventToEpoll(db_server_client) == -1)
            {
                close(db_server_client);
                return;
            }

            this->connections[db_server_client] = new RingBuffer();
            std::cout << "Connect to database server success! " + body->ip() + ":" + to_string(body->port()) << std::endl;
        }

        // 1s 后再次尝试重连一次
        for (Timer *const timer : m_callfuncList)
        {
            if (timer->GetType() == CallbackType::FuncServer_TryToConnectAvailabeServer)
            {
                timer->Start();
            }
        }
    }
}

bool FuncServer::OnListenerStart()
{
    // 定时发送自身信息给CenterServer
    this->SendSelfInfoToCenter();

    Timer *timer = new Timer(SEND_CENTERSERVER_TIME, CallbackType::FuncServer_SendSelfInfoToCenter, std::bind(&FuncServer::SendSelfInfoToCenter, this));
    timer->Start();

    m_callfuncList.push_back(timer);

    return true;
}

void FuncServer::HandleUserInfo(Message *msg, int fd)
{
    ServerProto::UserInfo *body = reinterpret_cast<ServerProto::UserInfo *>(msg->body->message);
    switch (body->opt()) 
    {
    case ServerProto::UserInfo_Operation_Register:
    {
        user_fd_record[msg->head->m_userid] = body->fd();
        break;
    }
    case ServerProto::UserInfo_Operation_Logout:
    {
        if (user_fd_record.count(msg->head->m_userid))
        {
            user_fd_record.erase(msg->head->m_userid);
        }
        break;
    }
    default:
    {
        break;
    }
    }
}
