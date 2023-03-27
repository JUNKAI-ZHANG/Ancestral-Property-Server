//
// Created by haowendeng on 2023/2/28.
//

#include "TcpManager.h"

#include <iostream>
#include <sys/socket.h>
#include <cerrno>
#include <arpa/inet.h>
#include <unistd.h>
#include "message_utils.h"

TcpManager::TcpManager() { init(); }

void TcpManager::init() {
    alive = true;
    server_fd = -1;
    connections.clear();
    fd2connid.clear();
    unused_connid.clear();
    for (int i = 1; i <= MAX_CONN_COUNT; i++) unused_connid.insert(i);

    ep_fd = epoll_create(1);

    if (ep_fd == -1) {
        std::cerr << "Create epfd failed" << std::endl;
        close();
    }
}

bool TcpManager::listen(int port) {
    return listen(port, false);
}

bool TcpManager::listen(int port, bool allow_reuse) {
    std::cout << "Starting tcp_manager" << std::endl;
    server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (server_fd == -1) {
        perror("socket failed");
        return false;
    }

    //可重用端口
    if (allow_reuse) {
        int ok = 1;
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &ok, sizeof(int)) == -1) {
            perror("setsockopt");
            ::close(server_fd);
            return false;
        }
    }

    // 绑定端口
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    int ret = bind(server_fd, (sockaddr *) &addr, sizeof(addr));
    if (ret == -1) {
        perror("bind failed");
        ::close(server_fd);
        return false;
    }

    // 监听端口
    ret = ::listen(server_fd, 8080);

    if (ret == -1) {
        perror("listen failed");
        ::close(server_fd);
        return false;
    }

    epoll_event event;
    event.data.u32 = server_fd;
    event.events = EPOLLIN | EPOLLERR;
    epoll_ctl(ep_fd, EPOLL_CTL_ADD, server_fd, &event);

    return true;
}

void TcpManager::close() {
    alive = false;
    if (server_fd != -1) ::close(server_fd);
    if (ep_fd != -1) ::close(ep_fd);
    init();
}

void TcpManager::receive_messages(std::queue<Message *> *message_queue, int timeout_ms) {
    epoll_event event;
    event.events = EPOLLIN | EPOLLERR | EPOLLHUP;

    int event_count = epoll_wait(ep_fd, events, MAX_CONN_COUNT + 1, timeout_ms);
    if (event_count == -1) {
        std::cerr << "epoll_wait returned -1!!!" << std::endl;
        std::cerr << "errno=" << errno << std::endl;
        return;
    }

#ifdef debug
    if (event_count > 0) std::cerr << "event_count: " << event_count << std::endl;
#endif

    for (int i = 0; i < event_count; i++) {
        int fd = events[i].data.fd;
#ifdef debug
        std::cerr << "epoll event occured: " << fd << ' ' << events[count].events << std::endl;
#endif
        if (fd == server_fd) {
            // 新的连接
            if (events[i].events & EPOLLIN) {
                int conn_id = new_connection();
                std::shared_ptr<TcpConnection> conn = nullptr;
                if (conn_id > 0) conn = connections[conn_id].connection;
                if (conn == nullptr) {
                    std::cout << "Connection count reach the limit! New connection refused" << std::endl;
                    sockaddr_in client_addr{};
                    socklen_t len = sizeof(client_addr);
                    int sockfd = accept(server_fd, (sockaddr *) &client_addr, &len);
                    ::close(sockfd);
                } else {
                    if (conn->accept_from(server_fd) == -1) {
                        std::cerr << "accept failed" << std::endl;
                    }
                    fd2connid[conn->get_fd()] = conn_id;
                    std::cout << "connect accepted, id=" << conn_id << std::endl;
                    event.data.fd = conn->get_fd();
                    epoll_ctl(ep_fd, EPOLL_CTL_ADD, event.data.fd, &event);
                    // 发送hello
                    send_heartbeat(conn_id);
                }
            }
            if (events[i].events & EPOLLERR) {
                std::cerr << "EPOLLERR on tcp_manager fd, closing tcp_manager!";
                close();
            }
        } else {
            int conn_id = get_conn_id(events[i].data.fd);
            if (conn_id <= 0) continue;
            std::shared_ptr<TcpConnection> conn = connections[conn_id].connection;
            Entity entity = connections[conn_id].entity;
            if (events[i].events & EPOLLIN) {
                int ret = conn->flush();
                // 好像对端关闭连接后会发送很多次epollin但是读不到东西，怪
                if (ret <= 0) {
                    if (ret < 0) std::cerr << "flush error: " << ret << std::endl;
                    epoll_ctl(ep_fd, EPOLL_CTL_DEL, events[i].data.fd, nullptr);
                    close_connection(conn_id);
                } else {
                    int len = conn->available_length();
                    while (len > 0) {
                        if (conn->read(buf, buffer_size) == 0) {
                            //虽然传入的是buffer_size，但还是判一下，省的以后改小了放不下然后炸了
                            conn->skip_package();
                        }
                        //todo: 考虑在messagehead加上是否为转发消息，如果是，则直接转发而不完全解包
                        Message *message = decode_message(buf, len);
                        message->head->conn_id = conn_id;
                        //暂时让被连接方发起heartbeat，连接方回应，暂时主要用于确认双方身份，这条消息不发给应用层了
                        if (message->head->message_id == MSG_HEARTBEAT) {
                            if (conn->get_type() == 2) send_heartbeat(conn_id);
                        } else {
                            message_queue->push(message);
                        }
                        len = conn->available_length();

                        // 没有确定过实体类型的连接收到的第一个包确认一下实体类型
                        // 之后需要加上身份验证
                        if (entity.type == ENTITY_UNKNOWN) {
                            int type = message->head->sender_type;
                            int id = message->head->sender_id;
                            //还没有登录，id强制设置
                            if (type == ENTITY_CLIENT) {
                                id = -conn_id;
                            }
                            set_entity_for_connection(conn_id, type, id);
                        }
                    }
                }
            }
            if ((events[i].events & EPOLLHUP) || (events[i].events & EPOLLERR)) {
                //理论上应该处理完剩余信息再断开，但是来不及写了
                std::cout << "shutdown connection! id=" << conn_id << std::endl;
                close_connection(conn_id);
            }
        }
    }
}

void TcpManager::broadcast(const Message *message) {
    if (!message->SerializeToArray(buf, buffer_size)) {
        std::cerr << "Broadcast serialize failed!" << std::endl;
        return;
    }
    if (entity_self.type != ENTITY_GATE) {
        message->head->sender_type = entity_self.type;
        message->head->sender_id = entity_self.id;
    }
    if (message->head->receiver_type == ENTITY_CLIENT && entity_self.type != ENTITY_GATE) {
        message->head->receiver_id = -1;
        send(message);
        return;
    }
    int size = (int) message->length();
    for (auto &p: connections) {
        auto conn = p.second.connection;
        int type = p.second.entity.type;
        if (type != message->head->receiver_type) continue;
        if (!conn->is_alive()) continue;
        conn->write(buf, size);
    }
}

int TcpManager::new_connection() {
    if (unused_connid.empty()) return -1;
    int id = *unused_connid.begin();
    unused_connid.erase(id);
    Container container;
    container.connection = std::make_shared<TcpConnection>();
    container.entity = {ENTITY_UNKNOWN, -id};
    connections[id] = container;
    entity2id[container.entity] = id;
    return id;
}

//没时间优化了，先遍历吧
int TcpManager::get_conn_id(int fd) const {
    if (fd2connid.count(fd))
        return fd2connid.find(fd)->second;
    else
        return -1;
}

std::shared_ptr<TcpConnection> TcpManager::get_connection(int type, int id) {
    //id < 0直接拿conn_id为-id的链接
    if (id < 0) {
        if (connections.count(-id) == 0) return nullptr;
        return connections[-id].connection;
    }
    Entity entity = {type, id};
    if (entity2id.count(entity)) return connections[entity2id[entity]].connection;
    else return nullptr;
}


TcpManager::Entity TcpManager::get_connection_entity(int conn_id){
    if (connections.count(conn_id) == 0) return {ENTITY_UNKNOWN, -1};
    return connections[conn_id].entity;
}

bool TcpManager::send(const Message *message) {
    if (entity_self.type != ENTITY_GATE) {
        message->head->sender_type = entity_self.type;
        message->head->sender_id = entity_self.id;
    }
    if (!message->SerializeToArray(buf, buffer_size)) return false;
    std::shared_ptr<TcpConnection> conn;
    if (message->head->receiver_type == ENTITY_CLIENT && entity_self.type != ENTITY_GATE) {
        conn = get_connection(ENTITY_GATE, 0);
    } else {
        conn = get_connection(message->head->receiver_type, message->head->receiver_id);
    }
    //为了客户端调试暂时改的
    if(entity_self.type == ENTITY_CLIENT && message->head->message_id == MSG_REQUEST_LOGIN){
        conn = get_connection(ENTITY_GATE, 0);
    }
    if (conn == nullptr) {
        std::cerr << "can not found connection type=" << message->head->receiver_type
                  << " ,id=" << message->head->receiver_id << std::endl;
        return false;
    }
    return conn->write(buf, (int) message->length());
}

bool TcpManager::send_raw(const Message *message) {
    if (!message->SerializeToArray(buf, buffer_size)) return false;
    std::shared_ptr<TcpConnection> conn = get_connection(message->head->receiver_type, message->head->receiver_id);
    if (conn == nullptr) return false;
    return conn->write(buf, (int) message->length());
}


void TcpManager::close_connection(int id) {
    if (connections.count(id) == 0) {
        return;
    }
    epoll_ctl(ep_fd, EPOLL_CTL_DEL, connections[id].connection->get_fd(), nullptr);
    entity2id.erase(connections[id].entity);
    fd2connid.erase(connections[id].connection->get_fd());
    connections[id].connection->close();
    connections.erase(id);
    unused_connid.insert(id);
}

bool TcpManager::connect_to(const char *ip, int port) {
    int conn_id = new_connection();
    if (conn_id <= 0) return false;
    std::shared_ptr<TcpConnection> conn = connections[conn_id].connection;
    if (conn->connect_to(ip, port) != 0) return false;

    fd2connid[conn->get_fd()] = conn_id;

    epoll_event event;
    event.events = EPOLLIN | EPOLLERR | EPOLLHUP;
    event.data.u32 = conn->get_fd();
    epoll_ctl(ep_fd, EPOLL_CTL_ADD, conn->get_fd(), &event);
    return true;
}

void TcpManager::set_entity_for_connection(int conn_id, int type, int id, bool force) {
    if (connections.count(conn_id) == 0) return;
    if (entity2id.count({type, id}) > 0) {
        int shutdown_id = entity2id[{type, id}];
        if (shutdown_id != conn_id && force) close_connection(shutdown_id);
        else return;
    }
    Container &container = connections[conn_id];
    entity2id.erase(container.entity);
    container.entity = {type, id};
    entity2id[container.entity] = conn_id;
}

void TcpManager::set_self_role(int type, int id) {
    entity_self = {type, id};
}

void TcpManager::send_heartbeat(int conn_id) {
    Message *message = new Message;
    message->head = new MessageHead;
    message->head->sender_type = entity_self.type;
    message->head->sender_id = entity_self.id;

    //我不用知道对面是啥，反正就给这个链接发就行了
    message->head->receiver_id = ENTITY_UNKNOWN;
    message->head->receiver_id = -conn_id;
    message->head->message_id = MSG_HEARTBEAT;
    message->body = new MessageBody;
    send(message);
    delete message;
}

bool TcpManager::send(int receiver_type, int receiver_id, int msg_type, google::protobuf::MessageLite *body) {
    Message msg;
    msg.head = new MessageHead;
    msg.head->message_id = msg_type;
    msg.head->receiver_type = receiver_type;
    msg.head->receiver_id = receiver_id;
    msg.body = new MessageBody;
    msg.body->message = body;
    bool ret = send(&msg);
    //msg析构前置空，不要删掉外面传进来的东西！
    msg.body->message = nullptr;
    return ret;
}
