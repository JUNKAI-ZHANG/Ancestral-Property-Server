//
// Created by haowendeng on 2023/2/24.
//

#include "TcpConnection.h"


void TcpConnection::init() {
    sockfd = -1;
    alive = false;
    read_data_len = 0;
    send_data_len = 0;
    last_heartbeat = 0;
    type = 0;
    memset(&addr, 0, sizeof(addr));
}

int TcpConnection::flush() {
    int ret = recv(sockfd, read_buf + read_data_len, buffer_size - read_data_len, 0);
    if (ret < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // 缓冲区读完了
            return 0;
        } else {
            // 有错误
            perror("raw read error");
            return ret;
        }
    } else {
        read_data_len += ret;
    }
    return ret;
}

void TcpConnection::close() {
    if (alive) shutdown(sockfd, SHUT_RDWR);
    init();
}

int TcpConnection::available_length() {
    // 如果连包长都没收完就返回
    if (read_data_len < length_size) return 0;
    // buf前两个字节一定是长度
    int len = 0;
    if (length_size == 2) len = ntohs(*(ushort *) read_buf);
    else if (length_size == 4) len = ntohl(*(ulong *) read_buf);
    // 还没收完一个包
    if (read_data_len - length_size < len) return 0;
    return len;
}

int TcpConnection::read(char *data, int len) {
    int avai_len = available_length();
    if (len < avai_len) return 0;
    memcpy(data, read_buf + length_size, avai_len);
    //删掉被读走的信息，这里要用memmove!因为可能会有交集
    memmove(read_buf, read_buf + avai_len + length_size, read_data_len - avai_len - length_size);
    read_data_len -= avai_len + length_size;
#ifdef debug
    std::cout << "read: " << avai_len << std::endl;
#endif
    return avai_len;
}

void TcpConnection::skip_package() {
    int avai_len = available_length();
    memmove(read_buf, read_buf + avai_len + length_size, read_data_len - avai_len - length_size);
    read_data_len -= avai_len + length_size;
}


int TcpConnection::write(char *data, int len) {
    if (send_data_len + len > buffer_size) {
        return -1;
    }
    if (length_size == 2) *(ushort *) (send_buf + send_data_len) = htons(len);
    else if (length_size == 4) *(ulong *) (send_buf + send_data_len) = htonl(len);
    send_data_len += length_size;
    memcpy(send_buf + send_data_len, data, len);
    send_data_len += len;
    return send();
}

int TcpConnection::send() {
    if (send_data_len == 0) return 0;
    int bytes = 0;
    int ret;
    while (bytes < send_data_len) {
        ret = ::send(sockfd, send_buf + bytes, send_data_len - bytes, 0);
        if (ret < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 缓冲区写不下了
                ret = 0;
                break;
            } else {
                // 有错误
                perror("raw read error");
                break;
            }
        } else bytes += ret;
    }
    memmove(send_buf, send_buf + bytes, send_data_len - bytes);
    send_data_len -= bytes;
    if (ret < 0) return ret;
#ifdef debug
    std::cout << "wrote: " << bytes << std::endl;
#endif
    return bytes;
}

int TcpConnection::connect_to(const char *ip, int port) {
    close();
    sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (sockfd == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    sockaddr_in server_addr{AF_INET, (in_port_t) htons(port), inet_addr(ip)};

    type = 2;

    if (connect(sockfd, (sockaddr *) &server_addr, sizeof(server_addr)) == -1 && errno != EINPROGRESS) {
        type = 0;
        perror("Failed to connect to tcp_manager: ");
        ::close(sockfd);
        return 1;
    }
    addr = server_addr;
    alive = true;
    return 0;
}

int TcpConnection::accept_from(int server_fd) {
    close();

    sockaddr_in client_addr{};
    socklen_t len = sizeof(client_addr);
    sockfd = accept(server_fd, (sockaddr *) &client_addr, &len);
    if (sockfd == -1) {
        perror("accept failed");
        return 1;
    }
    addr = client_addr;
    alive = true;
    type = 1;
    return 0;
}
