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
    Message *msg = NewServerInfoMessage("", 0, InType, ServerProto::ServerInfo_Operation_RequstAssgin, SERVER_FREE_LEVEL::FREE);

    SendMsg(msg, center_server_client);

    delete msg;
}

void FuncServer::OnConnectToCenterServer()
{
}

void FuncServer::SendSelfInfoToCenter()
{
    Message *msg = NewServerInfoMessage("127.0.0.1", this->listen_port, server_type, ServerProto::ServerInfo_Operation_Register, SERVER_FREE_LEVEL::FREE);

    if (msg == nullptr)
    {
        return;
    }

    // 如果发送失败 意味和center server断开连接 尝试重连
    if (!SendMsg(msg, center_server_client))
    {
        // 连接中心服务器
        if (!ConnectToOtherServer(center_ip, center_port, center_server_client))
        {
            std::cerr << "Failed to connect center server, boot it first" << std::endl;
            return;
        }

        if (conn_epoll->AddEventToEpoll(center_server_client) == -1)
        {
            close(center_server_client);
            return;
        }

        this->connections[center_server_client] = new RingBuffer();
        std::cout << "Connect to center server success!" << std::endl;

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

void FuncServer::OnMsgBodyAnalysised(Message *msg, const uint8_t *body, uint32_t length, int fd)
{
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

            if (conn_epoll->AddEventToEpoll(logic_server_client) == -1)
            {
                close(logic_server_client);
                return;
            }

            this->connections[logic_server_client] = new RingBuffer();
            std::cout << "Connect to logic server success!" << std::endl;;
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

            if (conn_epoll->AddEventToEpoll(db_server_client) == -1)
            {
                close(db_server_client);
                return;
            }

            this->connections[db_server_client] = new RingBuffer();
            std::cout << "Connect to database server success!" << std::endl;
        }

        Timer timer;
        // 1s 后再次尝试重连
        timer.startOnce(1000, &FuncServer::TryToConnectAvailabeServer, this);
    }
}

void FuncServer::HandleUserInfo(Message *msg, int fd)
{
    ServerProto::UserInfo *body = reinterpret_cast<ServerProto::UserInfo *>(msg->body->message);
    switch (body->opt()) 
    {
    case ServerProto::UserInfo_Operation_Register :
    {
        user_fd_record[msg->head->m_userid] = body->fd();
        break;
    }
    case ServerProto::UserInfo_Operation_Logout :
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
