//
// Created by haowendeng on 2023/3/2.
//

#include "logic_room.h"

Room::Room() {
    init();
}

void Room::init() {
    operate_in_tick.clear();
    frames_in_game.clear();
    players.clear();
    frame_count = 0;
    running = false;
}

bool Room::handle_player_operate(Message *message) {
    if (!running) return true;
    PlayerOperate *request = dynamic_cast<PlayerOperate *>(message->body->message);
    if (request == nullptr) {
        std::cerr << "PlayerOperate dynamic cast failed..." << std::endl;
        return false;
    }
    if (message->head->sender_id != request->uid()) return false;
    if (request->type() == OperateType::EnterRoom) {
        players.insert(request->uid());
        handle_reconnect(request->uid());
    }
    operate_in_tick.push_back(*request);
    return true;
}

void Room::handle_start_game() {
    if (running) return;
    init();
    running = true;
    LOGICFRAME.broadcast_to_client(MSG_NOTIFY_START_GAME, nullptr, players);
}

void Room::handle_end_game() {
    if (!running) return;
    operate_in_tick.clear();
    running = false;
    LOGICFRAME.broadcast_to_client(MSG_NOTIFY_STOP_GAME, nullptr, players);
}

void Room::handle_get_replay(Message *message) {
    GetReplay *request = dynamic_cast<GetReplay *>(message->body->message);
    if (request == nullptr) {
        std::cerr << "PlayerOperate dynamic cast failed..." << std::endl;
        return;
    }
    GameReplay replay;
    replay.mutable_frames()->Add(frames_in_game.begin(), frames_in_game.end());
    LOGICFRAME.send_message_to_client(request->uid(), MSG_RESPONSE_GAME_REPLAY, &replay);
}

void Room::handle_reconnect(int uid) {
    if (!running) return;
    if (players.count(uid) == 0) return;
    GameReplay replay;
    replay.mutable_frames()->Add(frames_in_game.begin(), frames_in_game.end());
    LOGICFRAME.send_message_to_client(uid, MSG_NOTIFY_RECONNECT_GAMEDATA, &replay);
}

void Room::tick() {
    if (!running) return;
    frame_count++;
    Frame frame;
    frame.set_frame_id(frame_count);
    frame.mutable_operates()->Add(operate_in_tick.begin(), operate_in_tick.end());
    operate_in_tick.clear();
    frames_in_game.push_back(frame);
    LOGICFRAME.broadcast_to_client(MSG_NOTIFY_OPERATE, &frame, players);
}
