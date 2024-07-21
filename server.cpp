#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<ctype.h>
#include<arpa/inet.h>
#include<string.h>
#include <iostream>
#include <thread>
using std::thread;
using std::cout;
using std::cin;
using std::endl;

void doHandler(int client_sock, struct sockaddr_in client_addr, int id) {
        char buf[1024];
        int n;
        char saddr[1024];


        printf("client connected:-IP:%s   -port:%d\n",
                inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, saddr, 1024),
                ntohs(client_addr.sin_port));
        fflush(stdout);
        fflush(stdin);
        while ((n = read(client_sock, buf, 1024))) {

                printf("get message:%s\n", buf);
                if (0 == strcmp(buf, "exit")) {
                        break;
                }

                memset(buf, 0, 1024);
        }
        cout << "client" << id << " disconnected." << endl;
        close(client_sock);
}


int main() {
        int server_sock = socket(AF_INET, SOCK_STREAM, 0);
        int client_total = 0;

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(8888);
        unsigned int dst;
        inet_pton(AF_INET, "127.0.0.1", &dst);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        int ret = bind(server_sock, (struct sockaddr*)&addr, sizeof(addr));
        atexit(release);
        if (ret == -1) {
                perror("bind error");
                exit(1);
        }
        ret = listen(server_sock, 5);//Max connections
        if (ret == -1) {

                perror("listen error");
                exit(1);
        }
        struct sockaddr_in comaddr;
        socklen_t addrlen = sizeof(sockaddr_in);
        client_total = 0;
        while (true) {
                int comfd;
                if (comfd = accept(server_sock, (struct sockaddr*)&comaddr, &addrlen)) {
                        thread t(doHandler, comfd, comaddr, client_total++);
                        t.detach();
                }

        }
        close(server_sock);
}
