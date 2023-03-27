#include "network/TcpManager.h"
#include "protobuf_types.h"

TcpManager tcpManager;

std::queue<Message *> message_queue;

// 登录请求gate直接扔给gameserver那边，其他直接透传
bool handle_request_login(Message *msg) {
    Login *request = dynamic_cast<Login *>(msg->body->message);
    if (request == nullptr) return false;
    LoginS2S *S2SRequest = new LoginS2S;
    S2SRequest->set_username(request->username());
    S2SRequest->set_password(request->password());
    S2SRequest->set_conn_id(msg->head->conn_id);
    Message message;
    message.head = new MessageHead;
    message.head->message_id = MSG_REQUEST_LOGIN_S2S;
    message.head->sender_id = msg->head->sender_id;
    message.head->sender_type = ENTITY_CLIENT;
    message.head->receiver_type = ENTITY_GAME_SERVER;
    message.head->receiver_id = 0;
    message.body = new MessageBody;
    message.body->message = S2SRequest;
    return tcpManager.send_raw(&message);
}

bool handle_response_login(Message *message) {
    LoginResultS2S *result = dynamic_cast<LoginResultS2S *>(message->body->message);
    LoginResult response;
    response.set_result(result->result());
    response.set_uid(result->uid());
    response.set_reason(result->reason());
    int conn_id = result->conn_id();
    int uid = result->uid();
    if (result->result()) {
        tcpManager.set_entity_for_connection(conn_id, ENTITY_CLIENT, uid);
        return tcpManager.send(ENTITY_CLIENT, uid, MSG_RESPONSE_LOGIN, &response);
    }
    return tcpManager.send(ENTITY_CLIENT, -conn_id, MSG_RESPONSE_LOGIN, &response);
}

bool dispatch_client_message(Message *message) {
    if (message->head->message_id == MSG_REQUEST_LOGIN) return handle_request_login(message);
    // 防止伪造uid
    if (message->head->sender_id < 0) return false;
    if (message->head->sender_id != tcpManager.get_connection_entity(message->head->conn_id).id) return false;
    return tcpManager.send(message);
}

bool dispatch_game_server_message(Message *message) {
    if (message->head->message_id == MSG_RESPONSE_LOGIN_S2S) return handle_response_login(message);
    if (message->head->receiver_type == ENTITY_CLIENT && message->head->receiver_id == -1) {
        tcpManager.broadcast(message);
        return true;
    }
    return tcpManager.send(message);
}

bool dispatch(Message *message) {
    switch (message->head->sender_type) {
        case ENTITY_CLIENT:
            return dispatch_client_message(message);
        case ENTITY_GAME_SERVER:
            return dispatch_game_server_message(message);
    }
    return false;
}

void handle_messages() {
    while (!message_queue.empty()) {
        Message *message = message_queue.front();
        message_queue.pop();

        if (!dispatch(message)) {
            std::cerr << "Message dispatch error. id=" << message->head->message_id << std::endl;
        }
        //message的空间要手动释放
        delete message;
    }
}

int main() {
    tcpManager.set_self_role(ENTITY_GATE, 0);
    tcpManager.listen(8080);
    tcpManager.connect_to("127.0.0.1", 8081);

    while (true) {
        tcpManager.receive_messages(&message_queue, 10);

        handle_messages();
    }

    return 0;
}