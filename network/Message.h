//
// Created by haowendeng on 2023/2/26.
//

#ifndef NET_MESSAGE_H
#define NET_MESSAGE_H

#include <arpa/inet.h>
#include "message_types.h"
#include "google/protobuf/message_lite.h"
#include "ByteStream.h"

class MessageHead {
public:
    short sender_type = ENTITY_UNKNOWN;
    int sender_id = -1;
    short receiver_type = ENTITY_UNKNOWN;
    int receiver_id = -1;
    int message_id = -1;

    //服务器自己加的conn_id，由server负责填写
    int conn_id = 0;

    static int length() {
        return sizeof(sender_type) + sizeof(sender_id) +
               sizeof(receiver_type) + sizeof(receiver_id) +
               sizeof(message_id);
    }

    MessageHead() = default;

    MessageHead(const char *data, int len) {
        if (len < length()) {
            sender_id = -1;
            message_id = MSG_ERR;
            return;
        }
        InputByteStream stream(data, len);

        stream >> sender_type >> sender_id
               >> receiver_type >> receiver_id
               >> message_id;

        if (!stream.is_available()) {
            sender_id = -1;
            message_id = MSG_ERR;
            return;
        }
    }

    bool SerailizeToArray(char *buffer, int len) const {
        if (length() > len) return false;
        OutputByteStream stream(buffer, len);
        stream << sender_type << sender_id
               << receiver_type << receiver_id
               << message_id;
        return stream.is_available();
    }
};

class MessageBody {
public:
    google::protobuf::MessageLite *message = nullptr;

    int length() const {
        if (message == nullptr) return 0;
        return (int) message->ByteSizeLong();
    }

    bool ParseFromArray(const char *data, int len) const {
        if (message == nullptr) return true;
        return message->ParseFromArray(data, len);
    }

    bool SerializeToArray(char *buffer, int len) const {
        if (message == nullptr) return true;
        return message->SerializeToArray(buffer, len);
    }

    ~MessageBody() {
        delete message;
    }
};

class Message {
public:
    MessageHead *head = nullptr;
    MessageBody *body = nullptr;

    int length() const {
        if (body == nullptr) return MessageHead::length();
        return MessageHead::length() + body->length();
    }

    bool SerializeToArray(char *buffer, int len) const {
        if (length() > len) return false;
        if (!head->SerailizeToArray(buffer, len)) return false;
        int head_length = MessageHead::length();
        if (body == nullptr) return true; //允许发送没有消息体的信息
        return body->SerializeToArray(buffer + head_length, len - head_length);
    }

    ~Message() {
        delete head;
        delete body;
    }
};

#endif //NET_MESSAGE_H
