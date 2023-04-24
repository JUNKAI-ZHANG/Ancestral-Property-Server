#include <thread>

#include "../Header/Server/ServerBase.h"

ServerBase::ServerBase()
{
    listen_epoll = new EpollMgr();

    std::thread th(std::bind(&ServerBase::SendMsgConsumer, this));
    th.detach();
}

ServerBase::~ServerBase()
{
    delete listen_epoll;

    CloseServer();
}

ServerProto::SERVER_TYPE ServerBase::TransformType(SERVER_TYPE server_type)
{
    if (server_type == SERVER_TYPE::GATE) return ServerProto::SERVER_TYPE::GATE;
    if (server_type == SERVER_TYPE::LOGIC) return ServerProto::SERVER_TYPE::LOGIC;
    if (server_type == SERVER_TYPE::DATABASE) return ServerProto::SERVER_TYPE::DATABASE;
    if (server_type == SERVER_TYPE::CENTER) return ServerProto::SERVER_TYPE::CENTER;
    if (server_type == SERVER_TYPE::NONE) return ServerProto::SERVER_TYPE::NONE;
    if (server_type == SERVER_TYPE::MATCH) return ServerProto::SERVER_TYPE::MATCH;
}

int ServerBase::StartListener(int port)
{
    // 创建 非阻塞监听socket
    listen_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (listen_fd == -1)
    {
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }

    int yes = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        return -1;
    }

    // 绑定地址和端口
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listen_fd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) == -1)
    {
        std::cerr << "Failed to bind address" << std::endl;
        close(listen_fd);
        return -1;
    }

    // 开始监听连接
    if (listen(listen_fd, SOMAXCONN) == -1)
    {
        std::cerr << "Failed to listen on socket" << std::endl;
        close(listen_fd);
        return -1;
    }

    listen_port = port;
    return 1;
}

void ServerBase::HandleListenerEvent(std::map<int, RingBuffer *> &conns, int fd)
{
    if (fd == listen_fd)
    {
        // 处理新连接
        struct sockaddr_in addr;
        socklen_t addrlen = sizeof(addr);
        int conn_fd = accept(listen_fd, reinterpret_cast<struct sockaddr *>(&addr), &addrlen);

        if (conn_fd == -1)
        {
            std::cerr << "Failed to accept connection" << std::endl;
            return;
        }

        std::cout << "Accepted connection from " << inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port) << std::endl;

        SendConnsChange(TransformType(server_type), listen_ip, listen_port, 1);

        // 将 conn_fd 注册到 conn_epoll 实例中
        struct epoll_event event;
        event.events = EPOLLIN;
        event.data.fd = conn_fd;

        if (listen_epoll->AddEventToEpoll(conn_fd) == -1)
        {
            CloseClientSocket(conn_fd);
        }
        else
        {
            conns[conn_fd] = new RingBuffer();
        }
    }
}

void ServerBase::SendConnsChange(ServerProto::SERVER_TYPE server_type, std::string ip, int port, int change)
{
    conns_count += change;
    if (server_type != ServerProto::SERVER_TYPE::CENTER)
    {
        SendToCenterServerConnChange(TransformType(ServerBase::server_type), listen_ip, listen_port, conns_count);
    }
}

void ServerBase::SendToCenterServerConnChange(ServerProto::SERVER_TYPE server_type, std::string ip, int port, int change)
{
    Message *message = new Message;

    ServerProto::ServerConnChange *body = new ServerProto::ServerConnChange;
    message->body = new MessageBody;
    body->set_ip(ip);
    body->set_port(port);
    body->set_change(change);
    body->set_type(server_type);
    message->body->message = body;

    message->head = new MessageHead(message->length(), BODYTYPE::ServerConnChange, 0);

    SendMsg(message, center_server_client);
    delete message;
}

void ServerBase::HandleConnEvent(std::map<int, RingBuffer *> &conn, int conn_fd)
{
    // 接收数据
    ssize_t body_size = 0;
    ssize_t tmp_received;

    // check connection state
    if (conn.find(conn_fd) == conn.end())
    {
        std::cerr << "Cannot find client buffer, Did the socket be closed" << std::endl;
        return;
    }

    // Receive data from client
    uint8_t tmp[JSON.TMP_BUFFER_SIZE];
    while ((tmp_received = recv(conn_fd, tmp, JSON.TMP_BUFFER_SIZE, MSG_DONTWAIT)) > 0)
    {
        if (!conn[conn_fd]->AddBuffer(tmp, tmp_received))
        {
            std::cerr << "ServerBase : Client Read Buffer is Full" << std::endl;
            tmp_received = -1;
            break;
        }
    }

    if (tmp_received < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            // 数据读取完毕
            HandleReceivedMsg(conn[conn_fd], conn_fd);
        }
        else
        {
            std::cerr << "ServerBase : Failed to read from connection" << std::endl;
            CloseClientSocket(conn_fd);
            return;
        }
    }
    else if (tmp_received == 0)
    {
        std::cout << "Connection closed by remote host" << std::endl;
        CloseClientSocket(conn_fd);
        return;
    }
}

void ServerBase::OnMsgBodyAnalysised(Message *msg, const uint8_t *body, uint32_t length, int fd)
{
    msg->body = CreateMessageBody(msg->head->m_packageType);

    // 不出意外，应该不会反序列化寄，因为已经判断过了。。。
    if (msg->body->ParseFromArray(body, length))
    {
    }
    else
    {
        std::cerr << "ServerBase : OnMsgBodyAnalysised ERROR" << std::endl;
    }
}

void ServerBase::HandleReceivedMsg(RingBuffer *buffer, int fd)
{
    auto buffer_size = buffer->GetCapacity();
    uint8_t *recv = buffer->GetBuffer(buffer_size);
    uint8_t *begin = recv;

    while (buffer_size >= JSON.HEAD_SIZE)
    {
        // Todo : 使用对象池对消息进行优化，这里new/delete太频繁了，等服务器跑起来就优化
        Message *message = new Message();
        message->head = new MessageHead(recv, JSON.HEAD_SIZE);

        // 判断反序列化是否正常
        if (CheckHeaderIsValid(message->head))
        {
            int pkgSize = message->head->m_packageSize;

            OnMsgBodyAnalysised(message, recv + JSON.HEAD_SIZE, pkgSize - JSON.HEAD_SIZE, fd);

            buffer_size -= pkgSize;

            // 指针向后偏移
            recv += pkgSize;
        }
        else
        {
            buffer_size = 0;
            std::cerr << "ServerBase : Parse Header Error" << std::endl;
        }
        int type = message->head->m_packageType;
        delete message;
    }

    // 清空缓冲区
    buffer->PopBuffer(buffer->GetCapacity());
    delete begin;
}

#ifdef MULTI_THREAD

bool ServerBase::SendMsg(Message *msg, int fd)
{
    MessagePair _pair;
    _pair.msg = new Message();
    *_pair.msg = *msg;
    _pair.fd = fd;
    _message_queue.Push(_pair);
}

void ServerBase::SendMsgConsumer()
{
    while (true)
    {
        usleep(100);
        MessagePair _pair;
        if (!_message_queue.TryPop(_pair)) continue;
        Message *msg = _pair.msg;
        int fd = _pair.fd;
        if (connections.count(fd))
        {
            int _size = msg->head->m_packageSize;
            if (_size > MAX_BUFFER_SIZE)
            {
                std::cerr << "Send size too big: " << _size << std::endl;
            }
            uint8_t resp[_size];
            if (!msg->SerializeToArray(resp, _size))
            {
                std::cerr << "ServerBase : Parse Msg Error (SendMsg)" << std::endl;
                return;
            }

            if (send(fd, resp, _size, 0) < 0)
            {
                std::cerr << "ServerBase : Could not send msg to client (SendMsg)" << std::endl;
                CloseClientSocket(fd);
                return;
            }

            return;
        }
        else
        {
            // std::cerr << "ServerBase : Client is not exist (SendMsg)" << std::endl;
            return;
        }
    }
}

#else

bool ServerBase::SendMsg(Message *msg, int fd)
{
    if (connections.count(fd))
    {
        int _size = msg->head->m_packageSize;
        if (_size > JSON.MAX_BUFFER_SIZE)
        {
            std::cerr << "Send size too big: " << _size << std::endl;
            return false;
        }
        uint8_t resp[_size];
        if (!msg->SerializeToArray(resp, _size))
        {
            std::cerr << "ServerBase : Parse Msg Error (SendMsg)" << std::endl;
            return false;
        }

        if (send(fd, resp, _size, 0) < 0)
        {
            std::cerr << "ServerBase : Could not send msg to client (SendMsg)" << std::endl;
            CloseClientSocket(fd);
            return false;
        }

        return true;
    }
    else
    {
        // std::cerr << "ServerBase : Client is not exist (SendMsg)" << std::endl;
        return false;
    }
}

void ServerBase::SendMsgConsumer()
{
    
}

#endif

bool ServerBase::OnListenerStart()
{
    // 默认为true,virtual需要子类重载
    return true;
}

void ServerBase::CloseClientSocket(int fd)
{
    if (connections.count(fd))
    {
        delete connections[fd];
        connections.erase(fd);

        close(fd);
        SendConnsChange(TransformType(server_type), listen_ip, listen_port, -1);
    }
}

void ServerBase::CloseServer()
{
    if (listen_fd > 0)
    {
        close(listen_fd);
    }
}

void ServerBase::Update()
{
    // 子类重写
}

void ServerBase::BootServer(int port)
{
    if (StartListener(port) == 1)
    {
        std::cout << "Server start at port : " << port << std::endl;

        if (listen_epoll->CreateEpoll() == -1)
        {
            close(listen_fd);
            return;
        }

        if (listen_epoll->AddEventToEpoll(listen_fd) == -1)
        {
            close(listen_fd);
            return;
        }

        std::cout << "Epoll create success, wait for event..." << std::endl;

        if (!OnListenerStart())
        {
            // 这期间如果出现异常 终止启动
            close(listen_fd);
            return;
        }

        if (false)
        {
            std::thread fires(std::bind(&ServerBase::FireAllEvents, this));
            fires.detach();
        }

        // 等待连接和数据
        struct epoll_event events[JSON.MAX_CLIENTS];

        while (true)
        {
            // 阻塞11ms监听
            int nfds = epoll_wait(listen_epoll->epoll_fd, events, JSON.MAX_CLIENTS, JSON.EPOLL_WAIT_TIME);
            if (nfds == -1)
            {
                std::cerr << "Failed to wait for events" << std::endl;
                break;
            }

            for (int i = 0; i < nfds; i++)
            {
                // address client socket data
                int conn_fd = events[i].data.fd;

                if (events[i].events & EPOLLIN)
                {
                    if (conn_fd == listen_fd) ServerBase::HandleListenerEvent(connections, conn_fd);
                    else ServerBase::HandleConnEvent(connections, conn_fd);
                }
                if (events[i].events & EPOLLERR)
                {
                    CloseClientSocket(conn_fd);
                }
                if (events[i].events & EPOLLHUP)
                {
                    CloseClientSocket(conn_fd);
                }
            }

            for (Timer *const _event : m_callfuncList)
            {
                _event->Tick();
            }
        }

        // 关闭 epoll 实例
        close(listen_epoll->epoll_fd);
    }
    else
    {
        close(listen_fd);
    }
}

void ServerBase::FireEvent()
{
    for (Timer *const _event : m_callfuncList)
    {
        _event->Tick();
    }
}

void ServerBase::FireAllEvents()
{
    Timer timer(11, CallbackType::ServerBase_FixedTimeFire, std::bind(&ServerBase::FireEvent, this));

    while (true)
    {
        timer.Tick();
    }
}

bool ServerBase::ConnectToOtherServer(std::string ip, int port, int &fd)
{
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        fd = -1;
        perror("socket");
        return false;
    }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &servaddr.sin_addr) <= 0)
    {
        fd = -1;
        perror("inet_pton");
        return false;
    }

    if (connect(fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        fd = -1;
        perror("connect");
        return false;
    }

    return true;
}

/* 返回当前时间戳的毫秒值 */
time_t ServerBase::getCurrentTime()
{
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    return now_ms.time_since_epoch().count();
}
