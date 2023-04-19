#include <thread>

#include "../Header/ServerBase.h"

ServerBase::ServerBase()
{
    listen_epoll = new EpollMgr();
}

ServerBase::~ServerBase()
{
    delete listen_epoll;

    CloseServer();
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
    while ((tmp_received = recv(conn_fd, tmp, TMP_BUFFER_SIZE, MSG_DONTWAIT)) > 0)
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
            // std::cout << "data read over" << std::endl;
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

    while (buffer_size >= HEAD_SIZE)
    {
        // Todo : 使用对象池对消息进行优化，这里new/delete太频繁了，等服务器跑起来就优化
        Message *message = new Message();
        message->head = new MessageHead(recv, HEAD_SIZE);

        // 判断反序列化是否正常
        if (CheckHeaderIsValid(message->head))
        {
            OnMsgBodyAnalysised(message, recv + HEAD_SIZE, message->head->m_packageSize - HEAD_SIZE, fd);

            buffer_size -= message->head->m_packageSize;

            // 指针向后偏移
            recv += message->head->m_packageSize;
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

bool ServerBase::SendMsg(Message *msg, int fd)
{
    if (connections.count(fd))
    {
        int _size = msg->head->m_packageSize;
        if (_size > MAX_BUFFER_SIZE)
        {
            std::cerr << "send size too big: " << _size << std::endl; 
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
        std::cerr << "ServerBase : Client is not exist (SendMsg)" << std::endl;
        return false;
    }
}

bool ServerBase::OnListenerStart()
{
    // 默认为true
    return true;
}

void ServerBase::CloseClientSocket(int fd)
{
    if (connections.count(fd))
    {
        delete connections[fd];
        connections.erase(fd);

        close(fd);
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

        // 等待连接和数据
        struct epoll_event events[MAX_CLIENTS];

        while (true)
        {
            // 阻塞11ms监听
            int nfds = epoll_wait(listen_epoll->epoll_fd, events, MAX_CLIENTS, 11);
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

            for (Timer * const _event : m_callfuncList)
            {
                _event->Tick();
            }
/*
            time_t nowTime = getCurrentTime();

            if (nowTime - STime >= 33)
            {
                STime = nowTime;
                Update();
            }
*/
        }

        // 关闭 epoll 实例
        close(listen_epoll->epoll_fd);

    }
    else
    {
        close(listen_fd);
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
