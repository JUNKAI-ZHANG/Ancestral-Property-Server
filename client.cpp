#include "network/TcpManager.h"
#include "time_util.h"
#include "protobuf/protobuf_types.h"

TcpManager tcpManager;
std::queue<Message *> message_queue;

const int maxn = 100000;
time_t time_start = 0;
int handled = 0;
int count = 0;
Login login;
int base = 1;

std::string randomStrGen(int length) {
    static std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
    std::string result;
    result.resize(length);

    for (int i = 0; i < length; i++)
        result[i] = charset[rand() % charset.length()];

    return result;
}

bool dispatch(Message *message) {
    if (message->head->message_id == MSG_RESPONSE_LOGIN) {
        handled++;
        if (handled == base) {
            std::cout << "handled=" << handled << " time=" << (getTimeMS() - time_start) << std::endl;
            base *= 10;
        }
    }
    return true;
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
    srand(time(NULL));
    tcpManager.set_self_role(ENTITY_CLIENT, 0);
    login.set_username("test");
    login.set_password("123");

    tcpManager.connect_to("10.0.128.157", 8080);
    sleep(1);
    time_start = getTimeMS();
    while (handled < maxn) {
        tcpManager.receive_messages(&message_queue, 10);

        handle_messages();
        if (count < maxn) {
            login.set_password(randomStrGen(10));
            count++;
            tcpManager.send(ENTITY_GAME_SERVER, 0, MSG_REQUEST_LOGIN, &login);
        }
    }
    return 0;
}