#ifndef _MESSAGE_UTILS_H
#define _MESSAGE_UTILS_H

#include "../Proto.h"
#include "Message.h"

static MessageBody *CreateMessageBody(int type)
{
    google::protobuf::MessageLite *message;
    switch (type)
    {
    case BODYTYPE::LoginRequest:
    {
        message = new LoginProto::LoginRequest;
        break;
    }
    case BODYTYPE::LoginResponse:
    {
        message = new LoginProto::LoginResponse;
        break;
    }
    case BODYTYPE::RegistRequest:
    {
        message = new LoginProto::RegistRequest;
        break;
    }
    case BODYTYPE::RegistResponse:
    {
        message = new LoginProto::RegistResponse;
        break;
    }
    case BODYTYPE::JoinRoom:
    {
        message = new RoomProto::JoinRoom;
        break;
    }
    case BODYTYPE::LeaveRoom:
    {
        message = new RoomProto::LeaveRoom;
        break;
    }
    case BODYTYPE::CreateRoom:
    {
        message = new RoomProto::CreateRoom;
        break;
    }
    case BODYTYPE::GetRoomList:
    {
        message = new RoomProto::GetRoomList;
        break;
    }
    case BODYTYPE::RoomStatusChangeRequest:
    {
        message = new RoomProto::RoomStatusChangeRequest;
        break;
    }
    case BODYTYPE::RoomStatusChangeResponse:
    {
        message = new RoomProto::RoomStatusChangeResponse;
        break;
    }
    case BODYTYPE::JoinGame:
    {
        message = new RoomProto::JoinGame;
        break;
    }
    case BODYTYPE::QuitGame:
    {
        message = new RoomProto::QuitGame;
        break;
    }
    case BODYTYPE::StartGame:
    {
        message = new FrameProto::StartGame;
        break;
    }
    case BODYTYPE::EndGame:
    {
        message = new FrameProto::EndGame;
        break;
    }
    case BODYTYPE::Frame:
    {
        message = new FrameProto::Frame;
        break;
    }
    case BODYTYPE::UserOperate:
    {
        message = new FrameProto::UserOperate;
        break;
    }
    case BODYTYPE::ChaseFrame: // 注意类型
    {
        message = new FrameProto::GameReplay;
        break;
    }
    case BODYTYPE::GameReplay:
    {
        message = new FrameProto::GameReplay;
        break;
    }
    case BODYTYPE::Reconnect:
    {
        message = new FrameProto::Reconnect;
        break;
    }
    case BODYTYPE::UserHeart:
    {
        message = new FrameProto::UserHeart;
        break;
    }
    case BODYTYPE::ServerInfo:
    {
        message = new ServerProto::ServerInfo;
        break;
    }
    case BODYTYPE::UserInfo:
    {
        message = new ServerProto::UserInfo;
        break;
    }
    case BODYTYPE::GetRegionInfoRequest:
    {
        message = new ServerProto::GetRegionInfoRequest;
        break;
    }
    case BODYTYPE::GetRegionInfoResponse:
    {
        message = new ServerProto::GetRegionInfoResponse;
        break;
    }
    case BODYTYPE::JoinRegionRequest:
    {
        message = new ServerProto::JoinRegionRequest;
        break;
    }
    case BODYTYPE::JoinRegionResponse:
    {
        message = new ServerProto::JoinRegionResponse;
        break;
    }
    case BODYTYPE::ServerConnChange:
    {
        message = new ServerProto::ServerConnChange;
        break;
    }
    default:
    {
        message = nullptr;
        break;
    }
    }

    MessageBody *body = new MessageBody;
    body->message = message;

    return body;
}

static Message *DecodeMessage(uint8_t *data, int len)
{
    Message *message = new Message;

    message->head = new MessageHead(data, HEAD_SIZE);

    message->body = CreateMessageBody(message->head->m_packageType);
    message->body->ParseFromArray(data + HEAD_SIZE, len - HEAD_SIZE);

    return message;
}

static bool CheckHeaderIsValid(MessageHead *header)
{
    if (header->m_packageSize < HEAD_SIZE)
    {
        return false;
    }

    if (header->m_packageSize > MAX_BUFFER_SIZE)
    {
        return false;
    }

    if (header->m_packageType < BODYTYPE::LoginRequest)
    {
        return false;
    }

    if (header->m_packageType > BODYTYPE::ErrorToken)
    {
        return false;
    }

    return true;
}

// 请手动释放返回值 // 可能存在内存泄漏问题。
static Message *NewLoginResponseMessage(bool ret, std::string msg, uint32_t token, int userid, int fd, const std::string& username)
{
    Message *message = new Message;

    LoginProto::LoginResponse *body = new LoginProto::LoginResponse;
    message->body = new MessageBody;
    body->set_result(ret);
    body->set_msg(msg);
    body->set_token(token);
    body->set_userid(userid);
    body->set_username(username);
    message->body->message = body;

    message->head = new MessageHead(message->length(), BODYTYPE::LoginResponse, fd);

    return message;
}

static Message *NewRegisterResponseMessage(bool ret, std::string msg, int fd)
{
    Message *message = new Message;

    LoginProto::RegistResponse *body = new LoginProto::RegistResponse;
    message->body = new MessageBody;
    body->set_result(ret);
    body->set_msg(msg);
    message->body->message = body;

    message->head = new MessageHead;
    message->head->m_packageSize = message->length();
    message->head->m_packageType = BODYTYPE::RegistResponse;
    message->head->m_userid = fd;

    return message;
}

// 请手动释放返回值 // 可能存在内存泄漏问题。
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

    message->head = new MessageHead(message->length(), BODYTYPE::ServerInfo, 0);

    return message;
}

static Message *NewUserInfoMessage(int userid, int fd, ServerProto::UserInfo_Operation opt)
{
    Message *message = new Message;

    ServerProto::UserInfo *body = new ServerProto::UserInfo;
    message->body = new MessageBody;
    body->set_userid(userid);
    body->set_fd(fd);
    body->set_opt(opt);
    message->body->message = body;

    message->head = new MessageHead(message->length(), BODYTYPE::UserInfo, 0);
    return message;
}

static uint8_t *IntToByte(const int x)
{
    uint8_t *ret = new uint8_t[4];
    ret[3] = ((x >> 0) & 255);
    ret[2] = ((x >> 8) & 255);
    ret[1] = ((x >> 16) & 255);
    ret[0] = ((x >> 24) & 255);
    return ret;
}

static uint8_t *UintToByte(const uint32_t x)
{
    uint8_t *ret = new uint8_t[4];
    ret[3] = ((x >> 0) & 255);
    ret[2] = ((x >> 8) & 255);
    ret[1] = ((x >> 16) & 255);
    ret[0] = ((x >> 24) & 255);
    return ret;
}
static int ByteToInt(const uint8_t *x)
{
    int ret = 0;
    ret = (ret << 8) + (int)x[0];
    ret = (ret << 8) + (int)x[1];
    ret = (ret << 8) + (int)x[2];
    ret = (ret << 8) + (int)x[3];
    return ret;
}
static uint ByteToUint(const uint8_t *x)
{
    uint ret = 0;
    ret = (ret << 8) + (uint)x[0];
    ret = (ret << 8) + (uint)x[1];
    ret = (ret << 8) + (uint)x[2];
    ret = (ret << 8) + (uint)x[3];
    return ret;
}

static std::string to_string(int x)
{
    std::string ret = "";
    while (x)
    {
        ret.push_back(x % 10 + '0');
        x /= 10;
    }
    reverse(ret.begin(), ret.end());
    return ret;
}

#endif