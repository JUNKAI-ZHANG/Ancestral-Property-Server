#include "../Header/Server/GateServer.h"

GateServer::GateServer()
{
    server_type = SERVER_TYPE::GATE;

    if (!JSON.PRESSURE_TEST)
    {
        // 注册检查心跳事件
        Timer *timer = new Timer(JSON.CHECK_ALL_CLIENT_CONN, CallbackType::GateServer_CheckAllClientConn, std::bind(&GateServer::CheckAllClientConn, this));
        timer->Start();

        m_callfuncList.push_back(timer);
    }
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
    else if (fd == db_server_client)
    {
        db_server_client = -1;

        TryToConnectAvailabeServer();
    }
    else
    {
        if (fd_user_record.count(fd))
        {
            Message *msg = NewUserInfoMessage(fd_user_record[fd], fd, ServerProto::UserInfo_Operation::UserInfo_Operation_Logout);
            SendMsg(msg, logic_server_client);
            int _fd = fd, _userid = fd_user_record[fd];
            user_fd_record.erase(_userid);
            fd_user_record.erase(_fd);
            delete msg;
        }
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

void GateServer::CheckAllClientConn()
{
    time_t nowMs = getCurrentTime();
    for (auto conn = m_user_connections.begin(); conn != m_user_connections.end();)
    {
        auto now = conn++;
        if (nowMs - now->second->m_lstMs > JSON.MAX_HEARTBEAT_DIFF)
        {
            CloseClientSocket(now->second->m_fd);
            m_user_connections.erase(now);
        }
    }
}

bool GateServer::CheckMessageValid(Message *msg, int fd)
{
    if (fd == center_server_client || fd == logic_server_client || fd == db_server_client)
    {
        return true;
    }
    if (msg->head->m_userid == 0)
    {
        int type = msg->head->m_packageType;
        if (type == BODYTYPE::LoginRequest || type == BODYTYPE::RegistRequest)
        {
            return true;
        }
    }
    else
    {
        if (fd == user_fd_record[msg->head->m_userid])
        {
            return true;
        }
    }
    return false;
}

void GateServer::OnMsgBodyAnalysised(Message *msg, const uint8_t *body, uint32_t length, int fd)
{
    FuncServer::OnMsgBodyAnalysised(msg, body, length, fd);

    if (!JSON.PRESSURE_TEST)
    {
        if (!CheckMessageValid(msg, fd))
        {
            return;
        }

        if (fd_user_record.count(fd))
        {
            // GateServer 根据fd将userid修改为信任值
            msg->head->m_userid = fd_user_record[fd];
        }
    }
    switch (msg->head->m_packageType)
    {
    case BODYTYPE::LoginRequest:
    {
        // 登录的m_userid字段暂时存储user_fd
        msg->head->m_userid = fd;

        // 转发给db server处理
        SendMsg(msg, db_server_client);
        break;
    }
    case BODYTYPE::LoginResponse:
    {
        LoginProto::LoginResponse *body = dynamic_cast<LoginProto::LoginResponse *>(msg->body->message);
        if (body == nullptr)
            return;

        if (body->result())
        {
            user_fd_record[body->userid()] = msg->head->m_userid;
            fd_user_record[msg->head->m_userid] = body->userid();

            /* 如果成功了，那么让logicServer添加userid-username到内存 */
            SendMsg(msg, logic_server_client);
        }

        SendMsg(msg, msg->head->m_userid);
        break;
    }
    case BODYTYPE::RegistRequest:
    {
        // 注册的m_userid字段暂时存储user_fd
        msg->head->m_userid = fd;

        // 转发给db server处理
        SendMsg(msg, db_server_client);
        break;
    }
    case BODYTYPE::RegistResponse:
    {
        // 无论成功与否，都转发给client，无需额外的操作
        SendMsg(msg, msg->head->m_userid);
        break;
    }
    case BODYTYPE::JoinRoom:
    {
        RoomProto::JoinRoom *body = dynamic_cast<RoomProto::JoinRoom *>(msg->body->message);
        if (body == nullptr)
            return;

        if (body->type() == RoomProto::JoinRoom::Type::JoinRoom_Type_REQUEST)
        {
            /* 转发给logic server处理 */
            SendMsg(msg, logic_server_client);
        }
        else if (body->type() == RoomProto::JoinRoom::Type::JoinRoom_Type_RESPONSE)
        {
            /* 转发给 client 处理 */
            SendMsg(msg, user_fd_record[msg->head->m_userid]);
        }
        else
        {
            std::cerr << "GateServer : Error JoinRoom Type" << std::endl;
        }
        break;
    }
    case BODYTYPE::LeaveRoom:
    {
        RoomProto::LeaveRoom *body = dynamic_cast<RoomProto::LeaveRoom *>(msg->body->message);
        if (body == nullptr)
            return;

        if (body->type() == RoomProto::LeaveRoom::Type::LeaveRoom_Type_REQUEST)
        {
            /* 转发给logic server处理 */
            SendMsg(msg, logic_server_client);
        }
        else if (body->type() == RoomProto::LeaveRoom::Type::LeaveRoom_Type_RESPONSE)
        {
            /* 转发给 client 处理 */
            SendMsg(msg, user_fd_record[msg->head->m_userid]);
        }
        else
        {
            std::cerr << "GateServer : Error LeaveRoom Type" << std::endl;
        }
        break;
    }
    case BODYTYPE::CreateRoom:
    {
        RoomProto::CreateRoom *body = dynamic_cast<RoomProto::CreateRoom *>(msg->body->message);
        if (body == nullptr)
            return;

        if (body->type() == RoomProto::CreateRoom::Type::CreateRoom_Type_REQUEST)
        {
            /* 转发给logic server处理 */
            SendMsg(msg, logic_server_client);
        }
        else if (body->type() == RoomProto::CreateRoom::Type::CreateRoom_Type_RESPONSE)
        {
            /* 转发给 client 处理 */
            SendMsg(msg, user_fd_record[msg->head->m_userid]);
        }
        else
        {
            std::cerr << "GateServer : Error CreateRoom Type" << std::endl;
        }
        break;
    }
    case BODYTYPE::GetRoomList:
    {
        RoomProto::GetRoomList *body = dynamic_cast<RoomProto::GetRoomList *>(msg->body->message);
        if (body == nullptr) return ;
        
        if (body->type() == RoomProto::GetRoomList::Type::GetRoomList_Type_REQUEST)
        {
            /* 转发给logic server处理 */
            SendMsg(msg, logic_server_client);
        }
        else if (body->type() == RoomProto::GetRoomList::Type::GetRoomList_Type_RESPONSE)
        {
            /* 转发给 client 处理 */
            SendMsg(msg, user_fd_record[msg->head->m_userid]);
        }
        else 
        {
            std::cerr << "GateServer : Error JoinRoom Type" << std::endl;
        }
        break;
    }
    case BODYTYPE::RoomStatusChangeRequest:
    {
        SendMsg(msg, logic_server_client);
        break;
    }
    case BODYTYPE::RoomStatusChangeResponse:
    {
        SendMsg(msg, user_fd_record[msg->head->m_userid]);
        break;
    }
    case BODYTYPE::JoinGame:
    {
        SendMsg(msg, logic_server_client);
        break;
    }
    case BODYTYPE::QuitGame:
    {
        SendMsg(msg, logic_server_client);
        break;
    }
    case BODYTYPE::StartGame:
    {
        if (fd == logic_server_client)
        {
            SendMsg(msg, user_fd_record[msg->head->m_userid]);
        }
        else 
        {
            SendMsg(msg, logic_server_client);
        }
        break;
    }
    case BODYTYPE::UserOperate:
    {
        SendMsg(msg, logic_server_client);
        break;
    }
    case BODYTYPE::Frame:
    {
        SendMsg(msg, user_fd_record[msg->head->m_userid]);
        break;
    }
    case BODYTYPE::Reconnect:
    {
        SendMsg(msg, user_fd_record[msg->head->m_userid]);
        break;
    }
    case BODYTYPE::ChaseFrame:
    {
        SendMsg(msg, user_fd_record[msg->head->m_userid]);
        break;
    }
    case BODYTYPE::UserHeart:
    {
        if (fd == center_server_client || fd == logic_server_client || fd == db_server_client)
        {
            break;
        }
        int userid = msg->head->m_userid;
        if (m_user_connections.count(userid))
        {
            m_user_connections[userid]->m_lstMs = getCurrentTime();
        }
        else 
        {
            m_user_connections[userid] = new ClientInfo(userid, user_fd_record[userid], getCurrentTime());
        }
        break;
    }
    default:
    {
        break;
    }
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        // printf("Usage: %s port\n", argv[0]);
        // return 1;
    }

    // int port = std::atoi(argv[1]);

    const int ports[] = {
        JSON.GATE_SERVER_PORT_1, 
        JSON.GATE_SERVER_PORT_2, 
        JSON.GATE_SERVER_PORT_3, 
        JSON.GATE_SERVER_PORT_4, 
        JSON.GATE_SERVER_PORT_5, 
        JSON.GATE_SERVER_PORT_6
    };

    GateServer gateServer;

    for (int i = 0; i < 6; i++)
    {
        // std::cout << "Start Gate Center Server ing..." << std::endl;
        gateServer.BootServer(ports[i]);
    }

    return 0;
}
