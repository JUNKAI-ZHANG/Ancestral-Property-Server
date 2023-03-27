//
// Created by haowendeng on 2023/3/2.
//

#ifndef HOMEWORK_NETWORK_LOGIC_ROOM_H
#define HOMEWORK_NETWORK_LOGIC_ROOM_H

#include "protobuf_types.h"
#include "Message.h"
#include "logic_frame.h"

class Room {
public:
    Room();

    void init();

    bool handle_player_operate(Message *message);

    void handle_start_game();

    void handle_end_game();

    void handle_get_replay(Message *message);

    void handle_reconnect(int uid);

    void tick();

    bool is_running() const { return running; }

private:
    int frame_count;
    std::vector<PlayerOperate> operate_in_tick;
    std::vector<Frame> frames_in_game;
    std::set<int> players;
    bool running;
};

#endif //HOMEWORK_NETWORK_LOGIC_ROOM_H
