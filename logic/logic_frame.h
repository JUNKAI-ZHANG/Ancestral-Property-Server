//
// Created by haowendeng on 2023/3/2.
//

#ifndef LOGICFRAME_H
#define LOGICFRAME_H

#include "singleton.h"
#include "TcpManager.h"

#define LOGICFRAME (*Singleton<LogicFrame>::get())

class LogicFrame {
public:
    LogicFrame();

    bool send_message_to_client(int uid, int msg_type, google::protobuf::MessageLite *message);

    void broadcast_to_client(int msg_type, google::protobuf::MessageLite *message, const std::vector<int> &uid_list);

    void broadcast_to_client(int msg_type, google::protobuf::MessageLite *message, const std::set<int> &uid_list);

    bool send_message_to_dbserver(int msg_type, google::protobuf::MessageLite *message);

    void set_self_id(int id) {
        tcp_manager->set_self_role(ENTITY_GAME_SERVER, id);
    };

    void serve(int port) const;

    void receive_messages(std::queue<Message *> *queue, int timeout_ms) const;

    void set_entity_for_connection(int conn_id, int type, int id);

    TcpManager *tcp_manager;
private:
    bool send_message(int receiver_type, int receiver_id, int msg_type, google::protobuf::MessageLite *message) const;

    void broadcast(int receiver_type, int msg_type, google::protobuf::MessageLite *message) const;

};


#endif //LOGICFRAME_H
