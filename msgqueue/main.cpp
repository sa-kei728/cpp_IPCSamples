#include <iostream>

#include <sys/ipc.h>
#include <sys/msg.h>

#include <vector>
#include <chrono>
#include <thread>

constexpr int LOOPCOUNT = 1000000;

// structure for message queue
struct mesg_buffer {
    long msg_type;
    int  msg_data;
};

void send(int msgid, mesg_buffer& senddata){
    msgsnd(msgid, &senddata, sizeof(senddata), 0);
}

void recv(int msgid, mesg_buffer& recvdata){
    msgrcv(msgid, &recvdata, sizeof(recvdata), 1, 0);
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]){
    key_t key = ftok("queue", 'a');
    int msgid = msgget(key, 0666 | IPC_CREAT);

    mesg_buffer senddata;
    senddata.msg_type = 1;
    senddata.msg_data = 1;
    mesg_buffer recvdata;
    auto start = std::chrono::system_clock::now();

    std::thread th{[&msgid, &senddata]{
        int cnt = 0;
        while(LOOPCOUNT > cnt++){
            send(msgid, senddata);
        }
    }};

    int cnt = 0;
    while(LOOPCOUNT > cnt++){
        recv(msgid, recvdata);
    }
    auto end = std::chrono::system_clock::now();
    th.join();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "duration average: " << (double)duration / (double)LOOPCOUNT << "[us]" << std::endl;

    // to destroy the message queue 
    msgctl(msgid, IPC_RMID, NULL); 
    return 0;
}