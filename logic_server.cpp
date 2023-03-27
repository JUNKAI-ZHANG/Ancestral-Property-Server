#include <iostream>
#include <queue>
#include <vector>
#include "network/TcpManager.h"
#include "protobuf/protobuf_types.h"
#include "logic_room.h"
#include "time_util.h"

Room room;

std::queue<Message *> message_queue;

bool handle_request_login(Message *message) {
    return LOGICFRAME.send_message_to_dbserver(MSG_REQUEST_LOGIN_S2S, message->body->message);
}

bool handle_response_login(Message *message) {
    LoginResultS2S *result = dynamic_cast<LoginResultS2S *>(message->body->message);
    int uid = result->uid();
//    std::cout << "result: " << result->result() << std::endl;
    bool ret = LOGICFRAME.send_message_to_client(uid, MSG_RESPONSE_LOGIN_S2S, result);
    if (result->result() && room.is_running()) room.handle_reconnect(uid);
    return ret;
}

bool dispatch_client_message(Message *message) {
    switch (message->head->message_id) {
        case MSG_REQUEST_LOGIN_S2S:
            return handle_request_login(message);
        case MSG_NOTIFY_OPERATE:
            room.handle_player_operate(message);
            return true;
        case MSG_NOTIFY_START_GAME:
            room.handle_start_game();
            return true;
        case MSG_NOTIFY_STOP_GAME:
            room.handle_end_game();
            return true;
        case MSG_REQUEST_GAME_REPLAY:
            room.handle_get_replay(message);
            return true;
    }
    return false;
}

bool dispatch_db_server_message(Message *message) {
    switch (message->head->message_id) {
        case MSG_RESPONSE_LOGIN_S2S:
            return handle_response_login(message);
    }
    return false;
}

bool dispatch(Message *message) {
    switch (message->head->sender_type) {
        case ENTITY_CLIENT:
            return dispatch_client_message(message);
        case ENTITY_DB_SERVER:
            return dispatch_db_server_message(message);
    }
    return false;
}

void handle_messages() {
    while (!message_queue.empty()) {
        Message *message = message_queue.front();
        message_queue.pop();

        if (!dispatch(message)) {
            std::cerr << "Message dispatch error." << std::endl;
        }
        //message的空间要手动释放
        delete message;
    }
}

int main() {
    LOGICFRAME.set_self_id(0);
    LOGICFRAME.serve(8081);

    time_t last_tick_time = getTimeMS();
    const int tick_time = 50;

    while (true) {
        LOGICFRAME.receive_messages(&message_queue, 10);

        handle_messages();

        time_t current_time = getTimeMS();
        if (current_time >= last_tick_time + tick_time) {
            last_tick_time = current_time;
            room.tick();
        }
    }

    return 0;
}
