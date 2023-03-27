//
// Created by haowendeng on 2023/2/26.
//

#ifndef MESSAGE_UTILS_H
#define MESSAGE_UTILS_H

#include "Message.h"
#include "message_types.h"
#include "protobuf_types.h"

MessageBody *create_message_body(int type) {
    google::protobuf::MessageLite *message;
    switch (type) {
        case MSG_REQUEST_LOGIN:
            message = new Login;
            break;
        case MSG_RESPONSE_LOGIN:
            message = new LoginResult;
            break;
        case MSG_NOTIFY_OPERATE:
            message = new PlayerOperate;
            break;
        case MSG_REQUEST_GAME_REPLAY:
            message = new GetReplay;
            break;
        case MSG_NOTIFY_RECONNECT_GAMEDATA:
        case MSG_RESPONSE_GAME_REPLAY:
            message = new GameReplay;
            break;
        case MSG_REQUEST_LOGIN_S2S:
            message = new LoginS2S;
            break;
        case MSG_RESPONSE_LOGIN_S2S:
            message = new LoginResultS2S;
            break;
        case MSG_ERR:
        default:
            message = nullptr;
            break;
    }
    MessageBody *body = new MessageBody;
    body->message = message;
    return body;
}

Message *decode_message(const char *data, int len) {
    Message *message = new Message;
    MessageHead *head = new MessageHead(data, len);
    int head_len = MessageHead::length();
    message->body = create_message_body(head->message_id);
    message->body->ParseFromArray(data + head_len, len - head_len);
    message->head = head;
    return message;
}

#endif //MESSAGE_UTILS_H
