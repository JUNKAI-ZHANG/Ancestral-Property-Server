#ifndef _NET_MESSAGE_H
#define _NET_MESSAGE_H

#include <iostream>
#include <arpa/inet.h>

#include "Profile.h"
#include "google/protobuf/message_lite.h"

class MessageHead
{
public:
    int m_packageSize = 0;
    int m_packageType = 0;
    int m_userid = 0;

    /*
    * @brief 服务器自己加的conn_id，由server负责填写
    */
    int conn_id = 0;

    MessageHead() = default;

    MessageHead(const uint8_t *buffer, int len);

    MessageHead(int packageSize, int packageType, int userid);

    /*
     * @brief 这里的 *buffer 是序列化后得到的结果
     */
    bool SerailizeToArray(uint8_t *buffer, int len) const;
};

class MessageBody
{
public:
    google::protobuf::MessageLite *message = nullptr;

    int length() const;

    bool ParseFromArray(const uint8_t *buffer, int len) const;

    bool SerializeToArray(uint8_t *buffer, int len) const;

    ~MessageBody();
};

class Message
{
public:
    MessageHead *head = nullptr;
    MessageBody *body = nullptr;

    Message();

    int length() const;

    bool SerializeToArray(uint8_t *buffer, int len) const;

    ~Message();
};

#endif