//
// Created by haowendeng on 2023/3/2.
//

#include "logic_frame.h"

LogicFrame::LogicFrame() {
    tcp_manager = new TcpManager();
    tcp_manager->set_self_role(ENTITY_GAME_SERVER, -1);
}

bool LogicFrame::send_message_to_client(int uid, int msg_type, google::protobuf::MessageLite *message) {
    return send_message(ENTITY_CLIENT, uid, msg_type, message);
}

void LogicFrame::broadcast_to_client(int msg_type, google::protobuf::MessageLite *message,
                                     const std::vector<int> &uid_list) {
    for (auto uid: uid_list) {
        send_message_to_client(uid, msg_type, message);
    }
}

void LogicFrame::broadcast_to_client(int msg_type, google::protobuf::MessageLite *message,
                                     const std::set<int> &uid_set) {
    for (auto uid: uid_set) {
        send_message_to_client(uid, msg_type, message);
    }
}

bool LogicFrame::send_message_to_dbserver(int msg_type, google::protobuf::MessageLite *message) {
    return send_message(ENTITY_DB_SERVER, 0, msg_type, message);
}

bool LogicFrame::send_message(int receiver_type, int receiver_id, int msg_type,
                              google::protobuf::MessageLite *message) const {
    return tcp_manager->send(receiver_type, receiver_id, msg_type, message);
}

void LogicFrame::broadcast(int receiver_type, int msg_type, google::protobuf::MessageLite *message) const {
    Message msg;
    msg.head = new MessageHead;
    msg.head->message_id = msg_type;
    msg.head->receiver_type = receiver_type;
    msg.head->receiver_id = -1; //广播信息
    msg.body = new MessageBody;
    msg.body->message = message;
    tcp_manager->broadcast(&msg);
    //不要删掉外面传进来的body
    msg.body->message = nullptr;
}

void LogicFrame::serve(int port) const {
    tcp_manager->listen(port, true);
}

void LogicFrame::receive_messages(std::queue<Message *> *queue, int timeout_ms) const {
    tcp_manager->receive_messages(queue, timeout_ms);
}

void LogicFrame::set_entity_for_connection(int conn_id, int type, int id) {
    tcp_manager->set_entity_for_connection(conn_id, type, id);
}
