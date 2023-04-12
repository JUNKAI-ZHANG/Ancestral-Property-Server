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
    case BODYTYPE::StartGame:
    {
        message = new FrameProto::StartGame;
        break;
    }
    case BODYTYPE::CloseGame:
    {
        message = new FrameProto::CloseGame;
        break;
    }
    case BODYTYPE::EnterGame:
    {
       message = new FrameProto::EnterGame;
       break;
    }
    case BODYTYPE::QuitGame:
    {
       message = new FrameProto::QuitGame;
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
    case BODYTYPE::ChaseFrame:
    {
        message = new FrameProto::ChaseFrame;
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

    // 规定 : 单包大小不会超过1024
    if (header->m_packageSize > 1024)
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
static Message *NewLoginResponseMessage(bool ret, std::string msg, uint32_t token, int userid, int fd)
{
    Message *message = new Message;

    LoginProto::LoginResponse *body = new LoginProto::LoginResponse;
    message->body = new MessageBody;
    body->set_result(ret);
    body->set_msg(msg);
    body->set_token(token);
    body->set_userid(userid);
    message->body->message = body;

    message->head = new MessageHead;
    message->head->m_packageSize = message->length();
    message->head->m_packageType = BODYTYPE::LoginResponse;
    message->head->m_userid = fd;

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

static Message *NewJoinRoomResponse(const std::string result, bool ret, int userid)
{
    Message *message = new Message;
    std::mt19937 rng(std::chrono::system_clock::now().time_since_epoch().count());

    RoomProto::JoinRoom *body = new RoomProto::JoinRoom;
    message->body = new MessageBody;
    body->set_ret(ret);
    body->set_result(result);
    body->set_seed(rng() % UINT32_MAX);
    body->set_type(RoomProto::JoinRoom::Type::JoinRoom_Type_RESPONSE);
    message->body->message = body;

    message->head = new MessageHead(message->length(), BODYTYPE::JoinRoom, userid);

    return message;
}

static Message *NewLeaveRoomResponse(const std::string result, bool ret, int userid)
{
    Message *message = new Message;

    RoomProto::LeaveRoom *body = new RoomProto::LeaveRoom;
    message->body = new MessageBody;
    body->set_ret(ret);
    body->set_result(result);
    body->set_type(RoomProto::LeaveRoom::Type::LeaveRoom_Type_RESPONSE);
    message->body->message = body;

    message->head = new MessageHead(message->length(), BODYTYPE::LeaveRoom, userid);

    return message;
}

static Message *NewCreateRoomResponse(const std::string result, int roomid, std::string roomname, bool ret, int userid, bool isRoomhost)
{
    Message *message = new Message;
    std::mt19937 rng(std::chrono::system_clock::now().time_since_epoch().count());

    RoomProto::CreateRoom *body = new RoomProto::CreateRoom;
    message->body = new MessageBody;
    body->set_result(result);
    body->set_roomid(roomid);
    body->set_roomname(roomname);
    body->set_ret(ret);
    body->set_type(RoomProto::CreateRoom::Type::CreateRoom_Type_RESPONSE);
    body->set_seed(rng() % UINT32_MAX);
    body->set_is_roomhost(isRoomhost);
    message->body->message = body;

    message->head = new MessageHead(message->length(), BODYTYPE::CreateRoom, userid);

    return message;
}

static Message *NewGetRoomListResponse(const std::string result, int *roomlist, int *people, std::string *roomname, int roomsize, bool ret, int userid)
{
    Message *message = new Message;

    RoomProto::GetRoomList *body = new RoomProto::GetRoomList;
    message->body = new MessageBody;
    body->set_ret(ret);
    body->set_size(roomsize);
    body->set_type(RoomProto::GetRoomList::Type::GetRoomList_Type_RESPONSE);
    for (int i = 0; i < roomsize; i++) 
    {
        RoomProto::RoomInfo *roomInfo = body->add_room_list();
        roomInfo->set_roomid(roomlist[i]);
        roomInfo->set_roomname(roomname[i]);
        roomInfo->set_people_count(people[i]);
    }
    message->body->message = body;

    message->head = new MessageHead(message->length(), BODYTYPE::GetRoomList, userid);

    return message;
}

static Message *NewStartOrCloseGameMessage(BODYTYPE bodytype, int userid, int roomid, int room_userid)
{
    Message *message = new Message;

    if (bodytype == BODYTYPE::StartGame)
    {
        FrameProto::StartGame *body = new FrameProto::StartGame;
        message->body = new MessageBody;
        body->set_roomid(roomid);
        body->set_userpid(room_userid);
        message->body->message = body;
    }
    if (bodytype == BODYTYPE::CloseGame)
    {
        FrameProto::CloseGame *body = new FrameProto::CloseGame;
        message->body = new MessageBody;
        body->set_roomid(roomid);
        message->body->message = body;
    }

    message->head = new MessageHead(message->length(), bodytype, userid);

    return message;
}

static Message *NewUserJoinRoomMessage(BODYTYPE bodytype, int userpid, int userid)
{
    Message *message = new Message;

    FrameProto::EnterGame *body = new FrameProto::EnterGame;
    message->body = new MessageBody;
    body->set_userpid(userpid);
    message->body->message = body;

    message->head = new MessageHead(message->length(), bodytype, userid);

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