#include <iostream>
#include <string>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <algorithm>
#include <vector>
#include <chrono>
#include <thread>

const std::string socket_name("test_socket");
constexpr int LOOPCOUNT = 1000000;

void send(int socket_fd, int senddata){
    [[maybe_unused]] ssize_t len = write(socket_fd, (void*)&senddata, sizeof(int));
}

void recv(int socket_fd, int recvdata){
    [[maybe_unused]] ssize_t len = read(socket_fd, (void*)&recvdata, sizeof(int));
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]){
    int socket_fd;
    sockaddr_un name;

    socket_fd = socket(AF_LOCAL, SOCK_STREAM, 0);   // same machine connection
    name.sun_family = AF_LOCAL;
    strcpy(name.sun_path, socket_name.c_str());
    std::thread th{[&socket_fd, &name]{
        int cnt = 0;
        int senddata = 1;
        while(LOOPCOUNT > cnt++){
            connect(socket_fd, reinterpret_cast<sockaddr*>(&name), SUN_LEN(&name));
            send(socket_fd, senddata);
        }
    }};

    int cnt = 0;
    int recvdata = 0;
    sockaddr_un client_name;
    socklen_t client_name_len;
    int client_socket_fd;

    /* server setting */
    bind(socket_fd, reinterpret_cast<sockaddr*>(&name), SUN_LEN(&name));
    auto start = std::chrono::system_clock::now();
    while(LOOPCOUNT > cnt++){
        listen(socket_fd, 5);
        client_socket_fd = accept(socket_fd, reinterpret_cast<sockaddr*>(&client_name), &client_name_len);
        recv(client_socket_fd, recvdata);
    } 
    auto end = std::chrono::system_clock::now();
    th.join();

    close(client_socket_fd);
    close(socket_fd);
    unlink(socket_name.c_str());

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "duration average: " << (double)duration / (double)LOOPCOUNT << "[us]" << std::endl;
    return 0;
}