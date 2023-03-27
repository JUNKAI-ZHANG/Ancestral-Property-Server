//
// Created by haowendeng on 2023/3/1.
//
#include "mysqllib/MySQLManager.h"
#include "model/User.h"
#include "network/TcpManager.h"
#include "protobuf_types.h"
#include "hiredis/hiredis.h"

MySQLManager sqlManager;
TcpManager tcpManager;
redisContext *redis;
const bool enable_redis = true;

std::queue<Message *> message_queue;

bool handle_request_login(Message *message) {
    LoginS2S *request = dynamic_cast<LoginS2S *>(message->body->message);
    LoginResultS2S result;
    redisReply *reply = nullptr;
    if (enable_redis) {
        reply = (redisReply *) redisCommand(redis, "GET HOMEWORK:USER:%s", request->username().c_str());
    }
    if (enable_redis && (reply == nullptr || redis->err)) {
        std::cerr << "Error: " << redis->errstr << std::endl;
        return false;
    }
    MySQLUser user;
    if (enable_redis && reply->type == REDIS_REPLY_STRING) {
        if (reply->len == 0) user.setNull();
        else {
            UserModel redisUser;
            redisUser.ParseFromString(reply->str);
            user.set_id(redisUser.id());
            user.set_username(redisUser.username());
            user.set_password(redisUser.password());
        }
        freeReplyObject(reply);
    } else {
        freeReplyObject(reply);
        user = sqlManager.fetchOne<MySQLUser>(
                sqlManager.getStatementFactory()
                        ->append("select id, username, password from user where username=")
                        .escape(request->username()).end()
        );
        if (enable_redis) {
            if (user.isNull())
                reply = (redisReply *) redisCommand(redis, "SET HOMEWORK:USER:%s %s EX %d", request->username().c_str(),
                                                    "", 10);
            else {
                UserModel redisUser;
                redisUser.set_id(user.id());
                redisUser.set_username(user.username());
                redisUser.set_password(user.password());
                reply = (redisReply *) redisCommand(redis, "SET HOMEWORK:USER:%s %s EX 30", request->username().c_str(),
                                                    redisUser.SerializeAsString().c_str());
            }
            freeReplyObject(reply);
        }
    }
    result.set_uid(-1);
    result.set_conn_id(request->conn_id());
    result.set_result(false);
    if (user.isNull()) result.set_reason("找不到该用户");
    else if (user.password() != request->password()) result.set_reason("密码错误");
    else {
        result.set_uid(user.id());
        result.set_result(true);
    }
    return tcpManager.send(
            message->head->sender_type,
            message->head->sender_id,
            MSG_RESPONSE_LOGIN_S2S,
            &result
    );
}

bool dispatch_game_server_message(Message *message) {
    switch (message->head->message_id) {
        case MSG_REQUEST_LOGIN_S2S:
            return handle_request_login(message);
    }
    return false;
}

bool dispatch(Message *message) {
    switch (message->head->sender_type) {
        case ENTITY_GAME_SERVER:
            return dispatch_game_server_message(message);
    }
    return false;
}

void handle_messages() {
    while (!message_queue.empty()) {
        Message *message = message_queue.front();
        message_queue.pop();

        if (!dispatch(message)) {
            std::cerr << "Message dispatch error. id=" << message->head->message_id << std::endl;
        }
        //message的空间要手动释放
        delete message;
    }
}

int main() {
    tcpManager.set_self_role(ENTITY_DB_SERVER, 0);
    tcpManager.connect_to("127.0.0.1", 8081); // gameserver
    redis = redisConnect("10.0.128.157", 6379);

    sqlManager.connect("10.0.128.157", "root", "711010", "demo");

    while (true) {
        tcpManager.receive_messages(&message_queue, 10);

        handle_messages();
    }

    return 0;
}