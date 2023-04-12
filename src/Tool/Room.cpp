#ifndef _ROOM_CPP
#define _ROOM_CPP

#include "../Header/Room.h"
#include "../Header/LogicServer.h"

Room::Room()
{

}

Room::~Room()
{

}

void Room::JoinRoom(Message *msg, int fd)
{
    Message *message;
    RoomProto::JoinRoom *body = reinterpret_cast<RoomProto::JoinRoom *>(msg->body->message);
    user_gate[msg->head->m_userid] = fd;
    if (user_room.count(msg->head->m_userid))
    {
        message = NewJoinRoomResponse("Already in room " + to_string(user_room[msg->head->m_userid]), false, msg->head->m_userid);
    }
    else if (room.count(body->roomid()))
    {
        if (room[body->roomid()].size() == 1)
        {
            room[body->roomid()].insert(msg->head->m_userid);
            user_room[msg->head->m_userid] = body->roomid();

            message = NewJoinRoomResponse("Join success!", true, msg->head->m_userid);
        }
        else
        {
            message = NewJoinRoomResponse("Full of people...", false, msg->head->m_userid);
        }
    }
    else
    {
        message = NewJoinRoomResponse("Room is not exist...", false, msg->head->m_userid);
    }
    SendMsg(message, fd);
    delete message;
}

void Room::LeaveRoom(Message *msg, int fd)
{
    Message *message;
    RoomProto::LeaveRoom *body = reinterpret_cast<RoomProto::LeaveRoom *>(msg->body->message);
    if (room.count(body->roomid()))
    {
        auto _room = room[body->roomid()];
        if (_room.size() == 2)
        {
            _room.erase(msg->head->m_userid);
            user_room.erase(msg->head->m_userid);

            message = NewLeaveRoomResponse("Exit success!", true, msg->head->m_userid);
        }
        else if (_room.size() == 1)
        {
            user_room.erase(msg->head->m_userid);
            room.erase(body->roomid());

            message = NewLeaveRoomResponse("Exit success, Room destroy", true, msg->head->m_userid);
        }
    }
    else
    {
        message = NewLeaveRoomResponse("Room is not exist...", false, msg->head->m_userid);
    }
    SendMsg(message, fd);
    delete message;
}

void Room::CreateRoom(Message *msg, int fd)
{
    Message *message;
    RoomProto::CreateRoom *body = reinterpret_cast<RoomProto::CreateRoom *>(msg->body->message);

    user_gate[msg->head->m_userid] = fd;

    int room_id = ++now_room_count;

    room[room_id].insert(msg->head->m_userid);
    user_room[msg->head->m_userid] = room_id;
    room_name[room_id] = body->roomname();

    message = NewCreateRoomResponse("Create room success!", room_id, body->roomname(), true, msg->head->m_userid, true);

    SendMsg(message, fd);
    delete message;
}

void Room::GetRoomList(Message *msg, int fd)
{
    Message *message;
    RoomProto::GetRoomList *body = reinterpret_cast<RoomProto::GetRoomList *>(msg->body->message);

    int _size = room.size();

    int roomlist[_size], people[_size], p = 0;
    std::string roomname[_size];
    for (const auto &it : room)
    {
        roomlist[p] = it.first;
        people[p] = it.second.size();
        roomname[p++] = room_name[it.first];
    }

    message = NewGetRoomListResponse("Get success!", roomlist, people, roomname, _size, true, msg->head->m_userid);

    SendMsg(message, fd);
    delete message;
}

void Room::UserLogout(ServerProto::UserInfo *body)
{
    int userid = body->userid();

    int roomid = user_room[userid];
    if (room[roomid].count(userid))
    {
        room[roomid].erase(userid);
        std::cout << "user delete" << std::endl;
        if (room[roomid].empty())
        {
            room.erase(roomid);
            // std::cout << "room delete" << std::endl;

            user_room.erase(userid);
            // std::cout << "user_room delete" << std::endl;

            room_name.erase(roomid);
            // std::cout << "roomname delete" << std::endl;

            room_total_frame.erase(roomid);
            // std::cout << "room_total_frame delete" << std::endl;

            room_frame.erase(roomid);
            // std::cout << "room_frame delete" << std::endl;

            user_gate.erase(userid);
            // std::cout << "user_gate delete" << std::endl;

            userid_userpid.erase(userid);
            // std::cout << "userid_userpid delete" << std::endl;

            room_framecount.erase(roomid);
            // std::cout << "room_framecount delete" << std::endl;

            broadcast_list.erase(roomid);
            // std::cout << "room remove from broadcast_list" << std::endl;
        }
    }
    else
    {
        std::cout << "User not in room..." << std::endl;
    }
}

void Room::AddUserOperate(Message *msg, int fd)
{
    user_gate[msg->head->m_userid] = fd;

    int roomid = user_room[msg->head->m_userid];
    room_frame[roomid].push(msg);
    room_total_frame[roomid].push_back(msg);
}

void Room::BroadCastMsg()
{
    // ToDo : 模拟加锁过程
    for (int _room : broadcast_list)
    {
        room_framecount[_room]++;

        Message *msg = new Message;
        msg->body = new MessageBody;
        FrameProto::Frame *body = new FrameProto::Frame;
        while (!room_frame[_room].empty())
        {
            Message *tmp = room_frame[_room].front();
            room_frame[_room].pop();

            FrameProto::UserOperate *tmP = reinterpret_cast<FrameProto::UserOperate *>(tmp->body->message);

            FrameProto::UserOperate *user_data = body->add_operates();

            for (int64_t i : tmP->data())
            {
                user_data->add_data(i);
            }
            user_data->set_opt(tmP->opt());
            user_data->set_userpid(tmP->userpid());
        }
        body->set_frame_id(room_framecount[_room]);
        msg->body->message = body;

        msg->head = new MessageHead(msg->length(), BODYTYPE::Frame, 0);

        for (int _userid : room[_room])
        {
            msg->head->m_userid = _userid;
            int fd = user_gate[_userid];
            SendMsg(msg, fd);
        }
        if (msg != nullptr)
        {
            delete msg;
        }
    }
}

void Room::NotifyUserJoin(int userid)
{
    for (int _userid : room[user_room[userid]])
    {
        Message *msg = NewUserJoinRoomMessage(BODYTYPE::EnterGame, userid_userpid[userid], _userid);
        SendMsg(msg, user_gate[_userid]);
        delete msg;
    }
}

void Room::NotifyRoomStart(Message *msg, int fd)
{
    FrameProto::StartGame *body = reinterpret_cast<FrameProto::StartGame *>(msg->body->message);

    int roomid = body->roomid();

    broadcast_list.insert(roomid);

    // 1p2p
    int id = 0;
    for (int _userid : room[roomid])
    {
        ++id;
        userid_userpid[_userid] = id;
        Message *msg = NewStartOrCloseGameMessage(BODYTYPE::StartGame, _userid, roomid, id);
        SendMsg(msg, fd);
        delete msg;

        NotifyUserJoin(_userid);
    }
}

void Room::SendMsg(Message *msg, int fd)
{
    LOGICSERVER.SendMsg(msg, fd);
}

#endif