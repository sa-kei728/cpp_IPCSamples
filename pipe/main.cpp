#include <iostream>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#include <algorithm>
#include <vector>
#include <chrono>
#include <thread>

constexpr int LOOPCOUNT = 1000000;

void send(int fd, int senddata){
    [[maybe_unused]] ssize_t len = write(fd, (void*)&senddata, sizeof(int));
}

void recv(int fd, int recvdata){
    [[maybe_unused]] ssize_t len = read(fd, (void*)&recvdata, sizeof(int));
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]){
    int ret = mkfifo("fifo", 0666);
    if(ret == -1) {
        std::cerr << "cannot create fifo 'fifo'." << std::endl;
    }

    int fd;
    if((fd=open("fifo", O_RDWR))==-1){
        std::cerr << "Couldn't open fifo." << std::endl;
        exit(-1);
    }

    std::thread th{[fd]{
        int cnt = 0;
        int senddata = 1;
        while(LOOPCOUNT > cnt++){
            send(fd, senddata);
        }
    }};

    int cnt = 0;
    int recvdata = 0;

    auto start = std::chrono::system_clock::now();
    while(LOOPCOUNT > cnt++){
        recv(fd, recvdata);
    } 
    auto end = std::chrono::system_clock::now();
    th.join();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "duration average: " << (double)duration / (double)LOOPCOUNT << "[us]" << std::endl;

    ret = remove("fifo");
    return 0;
}