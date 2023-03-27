//
// Created by haowendeng on 2023/2/28.
//

#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <sys/epoll.h>
#include <queue>
#include "Message.h"
#include "TcpConnection.h"

#define MAX_CONN_COUNT 1024

class TcpManager {
public:
    struct Entity {
        int type;
        int id;

        struct Hash {
            size_t operator()(const Entity &val) const {
                return std::hash<int>()(val.type) ^ std::hash<int>()(val.id);
            }
        };

        struct Equal {
            bool operator()(const Entity &p1, const Entity &p2) const {
                return p1.type == p2.type && p1.id == p2.id;
            }
        };
    };

    TcpManager();

    void set_self_role(int type, int id);

    void init();

    bool listen(int port);

    bool listen(int port, bool allow_reuse);

    bool connect_to(const char *ip, int port);

    void receive_messages(std::queue<Message *> *message_queue, int timeout_ms);

    bool is_alive() const { return alive; }

    void close();

    //id < 0直接拿conn_id为-id的链接
    std::shared_ptr<TcpConnection> get_connection(int type, int id);

    Entity get_connection_entity(int conn_id);

    //如果要改过去的值已经存在时，若force为true则把已经存在的连接关闭，否则直接返回
    void set_entity_for_connection(int conn_id, int type, int id, bool force = true);

    bool send(int receiver_type, int receiver_id, int msg_type, google::protobuf::MessageLite *body);

    bool send(const Message *message);

    // 不修改sender的类型和id，直接发送
    bool send_raw(const Message *message);

    void send_heartbeat(int conn_id);

    void broadcast(const Message *message);

    void close_connection(int id);
private:
    struct Container {
        Entity entity;
        std::shared_ptr<TcpConnection> connection;
    };

    int new_connection();

    int get_conn_id(int fd) const;

private:
    bool alive;
    int server_fd;
    int ep_fd;

    std::unordered_map<int, Container> connections; // conn_id -> conn

    std::unordered_map<Entity, int, Entity::Hash, Entity::Equal> entity2id; // type, id -> conn_id

    std::unordered_map<int, int> fd2connid; // conn_fd -> conn_id

    std::set<int> unused_connid;

    char buf[buffer_size];

    epoll_event events[MAX_CONN_COUNT + 1];

    Entity entity_self = {-1, -1};
};


#endif //TCPSERVER_H
