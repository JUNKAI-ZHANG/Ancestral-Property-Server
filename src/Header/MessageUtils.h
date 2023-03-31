#ifndef _MESSAGE_UTILS_H
#define _MESSAGE_UTILS_H

#include "Proto.h"
#include "Message.h"

static MessageBody *CreateMessageBody(int type)
{
    google::protobuf::MessageLite *message;
    switch (type)
    {
    case BODYTYPE::LoginRequest:
        message = new LoginProto::LoginRequest;
        break;
    case BODYTYPE::LoginResponse:
        message = new LoginProto::LoginResponse;
        break;
    default:
        message = nullptr;
        break;
    }

    MessageBody *body = new MessageBody;
    body->message = message;

    return body;
}

static Message *DecodeMessage(const uint8_t *data, int len)
{
    Message *message = new Message;

    message->head = new MessageHead(data, HEAD_SIZE);

    message->body = CreateMessageBody(message->head->m_packageType);
    message->body->ParseFromArray(data + HEAD_SIZE, len - HEAD_SIZE);

    return message;
}

static bool CheckHeaderIsValid(MessageHead *header)
{
    if (header->m_packageSize <= HEAD_SIZE)
    {
        return false;
    }

    // 规定 : 单包大小不会超过1024
    if (header->m_packageSize > 1024)
    {
        return false;
    }

    if (header->m_packageType < BODYTYPE::LoginRequest)
    {
        return false;
    }

    if (header->m_packageType > BODYTYPE::ServerInfo)
    {
        return false;
    }

    return true;
}

// 请手动释放返回值
static Message *NewLoginResponseMessage(bool ret, std::string msg, std::string userid, uint32_t token, LoginProto::LoginResponse_Operation opt)
{
    Message *message = new Message;

    LoginProto::LoginResponse *body = new LoginProto::LoginResponse;
    message->body = new MessageBody;
    body->set_result(ret);
    body->set_msg(msg);
    body->set_userid(userid);
    body->set_token(token);
    body->set_opt(opt);
    message->body->message = body;

    message->head = new MessageHead;
    message->head->m_packageSize = message->body->length();
    message->head->m_packageType = BODYTYPE::LoginResponse;

    return message;
}

// 请手动释放返回值
static Message *NewServerInfoMessage(std::string ip, int port, SERVER_TYPE type, ServerProto::ServerInfo_Operation opt, SERVER_FREE_LEVEL level)
{
    Message *message = new Message;

    ServerProto::ServerInfo *body = new ServerProto::ServerInfo;
    message->body = new MessageBody;
    body->set_ip(ip);
    body->set_port(port);
    body->set_server_type(type);
    body->set_opt(opt);
    body->set_server_free_level(level);
    message->body->message = body;

    message->head = new MessageHead;
    message->head->m_packageSize = message->body->length();
    message->head->m_packageType = BODYTYPE::ServerInfo;

    return message;
}

#endif